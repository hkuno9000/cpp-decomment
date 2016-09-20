# cpp-decomment
remove C++ comments and white spaces in source files.

## Introduction
This tool remove C++ comments and white spaces in source files.
* remove C style block comment: "/* ... */" 
* remove C++ sytle single line comment: "//....."
* remove white space. 
* output stdout or a file "\<input-source-name\>.decomment"

### 日本語解説(japanese introduction)
このツールは、ソースファイルから、C++のコメントと空白を除去します.
コンパイルに影響を与えない余白をソースから除去することにより、
動作に影響するコード修正内容や、コード量の計測に役立ちます.

## Usage
```
decomment [-h?bimqnsr] [-d<DIR>] file1.cpp file2.cpp ...
  -h -?      this help
  -b         keep blank line
  -i         keep indent spaces
  -m         keep minimum space
  -q         remove quoted string
  -n         print line number of input-file
  -s         output to stdout instend of output-files
  -r         [WIN32 only] recursive search under the input-file's folder(wildcard needed)
  -d<DIR>    output to DIR
  fileN.cpp  input-files. wildcard OK. The output-file is 'fileN.cpp.decomment'
```
## install
```
git clone http https://github.com/hkuno9000/cpp-decomment.git
cd cpp-decomment
make
sudo make install
```

## History
* Automatically exported from code.google.com/p/cpp-decomment.
  * see https://code.google.com/archive/p/cpp-decomment/
* Change from New BSD License(BSD 3-Clause License) to MIT License.

## License
MIT
