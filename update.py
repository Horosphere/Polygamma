# Author: Horosphere
#
# Execute this script each time you introduce a new source or Qt moc header
# file to the src/ directory. Currently do not support form files.
#
# Feed the script with the .pro file name if you have multiple .pro files.
#
# We don't know what would happen if there are no header files/source files

import os, sys

os.chdir(os.path.dirname(os.path.realpath(__file__)));
source_directory = "src"
signature_begin = "# Auto-generated. Do not edit. All changes will be undone\n"
signature_end = "# Auto-generated end\n"

# Retroactively line continued to prevent \ from appearing on the last line
headers_begin = "HEADERS += "
sources_begin = "\nSOURCES += "
autoGen_padding = "\\\n\t" + source_directory

fileName = ""
if len(sys.argv) >= 2:
    fileName = sys.argv[1]
else:
    for file in os.listdir(os.getcwd()):
        if file.endswith(".pro"):
            fileName = str(file)
        else:
            continue

print("qmake project: " + fileName)

projectFile = [] # Lines of project file
iBegin = -1 # Indices corresponding to auto-generated sections
iEnd = -1
with open(fileName, "r") as file:
    index = 0
    for line in file:
        projectFile.append(line)
        if line == signature_begin:
            iBegin = index
        elif line == signature_end:
            iEnd = index
        index += 1

if iBegin == -1 or iEnd == -1 or iBegin > iEnd:
    print("No auto-generated region found in " + fileName + ".")
    print("Please check that the following lines exist:")
    print(signature_begin[:-1])
    print(signature_end[:-1])
    sys.exit(1)

del projectFile[iBegin + 1: iEnd]

autoGenHeaders = headers_begin
autoGenSources = sources_begin
# Scan src/ directory recursively to gather source files
for root, subdirs, files in os.walk("./" + source_directory): 
    for name in files:
        # Truncated filename. This is a legacy from CMakeLists generator
        f = str(os.path.join(root, name))[len(source_directory) + 2:]
        if f.endswith(('.h', '.hh', '.hpp')):
            print("Header: " + f)
            autoGenHeaders += autoGen_padding + f
        elif f.endswith(('c', '.cc', '.cpp')):
            print("Source: " + f)
            autoGenSources += autoGen_padding + f
        else:
            continue

autoGen = autoGenHeaders + autoGenSources + "\n"
projectFile.insert(iBegin + 1, autoGen)

with open(fileName, "w") as file:
    for line in projectFile:
        file.write(line)


