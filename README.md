# cpp-decomment
remove C++ comments and white spaces in source files.

## Introduction
This tool remove C++ comments and white spaces in source files.
* remove C style block comment: "/* ... */" 
* remove C++ sytle single line comment: "//....."
* remove white spaces.
* remove text in string literal (optional)
* output stdout or a file "\<input-source-name\>.decomment"

### 日本語解説(Japanese introduction)
このツールは、ソースファイルから、C++のコメントと空白を除去します.
コンパイルに影響を与えない余白をソースから除去することにより、
動作に影響するコード修正内容や、コード量の計測に役立ちます.

## Usage
```
decomment [-h?bimqgnsr] [-d<DIR>] file1.cpp file2.cpp ...
  version: 2.0
  -h -?      this help
  -b         keep blank line
  -i         keep indent spaces
  -m         keep minimum space
  -q         remove quoted string
  -g         ignore backslash at end of single line comment
  -n         print line number of input-file
  -s         output to stdout instend of output-files
  -r         [WIN32 only] recursive search under the input-file's folder(wildcard needed)
  -d<DIR>    output to DIR
  fileN.cpp  input-files. wildcard OK. The output-file is 'fileN.cpp.decomment'
```
## Install
```
git clone https://github.com/hkuno9000/cpp-decomment.git
cd cpp-decomment
make
sudo make install
```

## History

Sep.2016: version 2.0
* change from New BSD License(BSD 3-Clause License) to MIT License.
* add -g option
* support C++11 raw string literal. R"opt(...)opt"

Mar.2015
* Automatically exported from code.google.com/p/cpp-decomment.
  * see https://code.google.com/archive/p/cpp-decomment/

## License
MIT
