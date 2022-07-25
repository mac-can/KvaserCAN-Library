#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
#
# Software for Industrial Communication, Motion Control and Automation
#
# Copyright (c) 2017,2022 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
# All rights reserved.
#
# This program is dual-licensed under the BSD 2-Clause "Simplified" License and
# under the GNU General Public License v3.0 (or any later version).
# You can choose between one of them if you use this program.
#
# BSD 2-Clause Simplified License:
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS PROGRAM IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS PROGRAM, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# GNU General Public License v3.0 or later:
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
"""
    Scanner for test case annotations in test implementation files

    This program scans for for test case annotations in test source
    code files to generate the test specification.

    Test Case Annotations:

    // @xctest TC<n>: <subject>
        or
    // @testcase TC<n>: <subject>
    // @expected <value>
    // @pre:
    // @test:
    // @post:
    // @sub(<n>): <value>
    // @- <test-step>
    // @-- <test-step>
    // @note:
    // @todo:
    // @issue(<project>):
    // @workaround:
    // @end

    $Author: makemake $

    $Rev: 792 $
"""
# encoding: utf-8
from datetime import datetime
import argparse
import os

version = '0.1.2'

scanner = '=== Generated ' + datetime.today().strftime('%Y-%m-%d %H:%M:%S') + ', (c) by UV Software, Berlin ==='

testpath = '.'
testext = '.mm'
specpath = '.'
specext = '.txt'
xctest = False

# output the test case subject
def write_test_subject(file, text, option):
    if option['subject']:
        separator = ''.join(['~']*len(text))
        if option['separator']:
            file.write(separator + '\n')
        file.write(text + '\n')
        if option['separator']:
            file.write(separator + '\n')


# output a test case annnotation
def write_test_annotation(file, text, option):
    if option['annotation']:
        file.write(text + '\n')


# output the test case procedure name
def write_test_procedure(file, text, option, suite):
    if option['procedure']:
        if option['annotation']:
            file.write('\ntestcase: ' + text + '\n')
        else:
            file.write(suite + '::' + text + '\n')


# output options
testopts = { 'subject': True,
             'separator': True,
             'annotation': True,
             'procedure': False }


# parse the command line
parser = argparse.ArgumentParser()
parser.add_argument('testpath', type=str, nargs='?', default=testpath,
                    help='path to test implementation files, default=\'' + testpath + '\'')
parser.add_argument('-o', '--output', type=str, nargs='?', default=specpath,
                    help='path for test specification files, default=\'' + specpath + '\'')
parser.add_argument('-e', '--extension', type=str, nargs='?', default=testext,
                    help='extension of test implementation files, default=\'' + testext + '\'')
parser.add_argument('--testcase_only', action='store_true', help='output only the test case subject')
parser.add_argument('--testproc_name', action='store_true', help='output also the test procedure name')
parser.add_argument('--testproc_only', action='store_true', help='output only the test procedure name')

args = parser.parse_args()
testpath = args.testpath.replace('~', os.path.expanduser('~'), 1)
specpath = args.output.replace('~', os.path.expanduser('~'), 1)
testext = args.extension

if args.testcase_only:
    testopts['subject'] = True
    testopts['separator'] = False
    testopts['annotation'] = False
    testopts['procedure'] = False
elif args.testproc_name and not args.testcase_only:
    testopts['subject'] = True
    testopts['separator'] = True
    testopts['annotation'] = True
    testopts['procedure'] = True
elif args.testproc_only and not args.testcase_only:
    testopts['subject'] = False
    testopts['separator'] = False
    testopts['annotation'] = False
    testopts['procedure'] = True


# Scanner for test case annotations in test implementation files
print('scantest - scan for test case annotations, version ' + version)
total = 0
files = [f for f in os.listdir(testpath) if f.endswith(testext)]
for file in files:
    file1 = testpath + '/' + file
    file2 = specpath + '/' + os.path.splitext(file)[0] + specext
    try:
        with open(file1, 'r') as fp1, open(file2, 'w') as fp2:
            count = 0
            for line in fp1:
                line = line.strip()
                # test case annotation:
                if line.startswith('// @'):
                    # - test case subject (xctest)
                    if line.startswith('// @xctest '):
                        write_test_subject(fp2, line.replace('// @xctest ', '', 1), testopts)
                        xctest = True
                        count += 1
                        total += 1
                    # - test case subject (others)
                    elif line.startswith('// @testcase '):
                        write_test_subject(fp2, line.replace('// @xctest ', '', 1), testopts)
                        xctest = False
                        count += 1
                        total += 1
                    # - expected value
                    elif line.startswith('// @expected '):
                        write_test_annotation(fp2, line.replace('// @expected ', 'expected: ', 1), testopts)
                    # - test segments
                    elif line.startswith('// @pre:') or \
                         line.startswith('// @test:') or \
                         line.startswith('// @post:'):
                        write_test_annotation(fp2, '\n' + line.replace('// @', '', 1), testopts)
                    # - sub test cases
                    elif line.startswith('// @sub('):
                        write_test_annotation(fp2, line.replace('// @sub(', '- sub(', 1), testopts)
                    # - test steps (or other key words)
                    else:
                        write_test_annotation(fp2, line.replace('// @', '', 1), testopts)
                # xctest implemantation method
                elif line.startswith('- (void)test') and xctest is True:
                    write_test_procedure(fp2, line.replace('- (void)', '').replace(' {', ''), testopts, os.path.splitext(file)[0])
                # google test/mock fixture
                # TODO: insert coin here
            if testopts['separator']:
                fp2.write(scanner + '\n')
        print('Scan test implementation file: {}'.format(file1))
        print('{:4d} test specification(s) written into {}'.format(count, file2))
    except IOError as e:
        print('Operation failed: ' + e.strerror)
print('{:4d} test case(s) found in {}'.format(total, testpath))
