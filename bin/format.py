#!/usr/bin/env python

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import subprocess
import argparse
import json
import os

plugins_to_process = ["Aeon", "ModularGasGameplayActors", "RuleRanger"]

parser = argparse.ArgumentParser(description="Unreal Source Code Formatter")

parser.add_argument('--verbose', action='store_true', help='Increase output verbosity')
parser.add_argument('--dry-run', action='store_true', help='Should run in dry run mode and emit errors')
parser.add_argument('files', type=str, nargs='*', help='The file to analyze')

args = parser.parse_args()

if args.verbose:
    if args.dry_run:
        print(f"Performing Source Code Checking. Files: {args.files}")
    else:
        print(f"Performing Source Code Formatting. Files: {args.files}")


def format_json(filename, dry_run):
    with open(filename, "r") as file:
        original_data = file.read()
    new_data = json.dumps(json.loads(original_data), indent=2) + "\n"
    not_matches = f"{new_data}" != f"{original_data}"

    if not dry_run and not_matches:
        with open(filename, "w") as file:
            file.write(new_data)

    return not_matches


exitcode = 0

try:
    index_files = subprocess.check_output(["git", "diff-index", "--cached", "--name-only", "HEAD", *args.files],
                                          universal_newlines=True).splitlines()
    tree_files = subprocess.check_output(["git", "ls-tree", "-r", "--name-only", "HEAD", *args.files],
                                         universal_newlines=True).splitlines()
    files = tree_files + index_files
    files_to_format = []
    files_to_format_assuming_json = []
    for file in files:
        if not os.path.exists(file):
            # File does not exist. Probably means that the index list
            # includes deleted files. We just skip it
            pass
        elif file.lower().endswith(".uproject"):
            files_to_format_assuming_json.append(file)
        elif file.lower().endswith(".h") or file.lower().endswith(".cpp") or file.lower().endswith(".cs"):
            if file.startswith("Source/"):
                files_to_format.append(file)
            elif file.startswith("Plugins/"):
                for plugin in plugins_to_process:
                    if file.startswith(f"Plugins/{plugin}/Source/"):
                        if file.lower().endswith(".uplugin"):
                            files_to_format_assuming_json.append(file)
                        else:
                            files_to_format.append(file)

    files_to_format = list(set(files_to_format))
    files_to_format.sort()
    files_to_format_assuming_json = list(set(files_to_format_assuming_json))
    files_to_format_assuming_json.sort()

    if args.dry_run:
        for file in files_to_format_assuming_json:
            if format_json(file, True):
                print(f"File {file} is not formatted correctly.")
                exitcode = 1

        if 0 != len(files_to_format):
            result = subprocess.run(["clang-format", " --dry-run", "--Werror", "-i", *files_to_format])
            if 0 != result.returncode:
                print(result.stderr)
                exitcode = result.returncode
    else:
        for file in files_to_format_assuming_json:
            format_json(file, False)

        if 0 != len(files_to_format):
            subprocess.run(["clang-format", "-i", *files_to_format], check=True)

    if args.verbose:
        if 0 != len(files_to_format_assuming_json):
            print("Processed the following files as JSON:")
            for file in files_to_format_assuming_json:
                print(file)
        if 0 != len(files_to_format):
            print("Processed the following files:")
            for file in files_to_format:
                print(file)

    exit(exitcode)
except subprocess.CalledProcessError as e:
    print(f"Error executing process: {e}")
    exit(e.returncode)
except Exception as e:
    print(f"An error occurred: {e}")
    exit(-1)
