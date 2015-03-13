
## Introduction: ##
This tool remove C++ comments and white spaces in source files.
  * remove C style block comment: `"/* ... */"`
  * remove C++ sytle single line comment: `"//....."`
  * remove white space.
  * output stdout or a file `"<input-source-name>.decomment"`

### 日本語解説 ###
このツールは、ソースファイルから、C++のコメントと空白を除去します.
コンパイルに影響を与えない余白をソースから除去することにより、開発量や修正規模の見積もりに役立ちます.

## Usage: ##
```
decomment [-h?biqnsr] [-d<DIR>] file1.cpp file2.cpp ...
  -h -?      this help
  -b         keep blank line
  -i         keep indent spaces
  -q         remove quoted string
  -n         print line number of input-file
  -s         output to stdout instend of files(*.decomment)
  -r         recursive search under the input-file's folder(wildcard needed)
  -d<DIR>    output to DIR
  fileN.cpp  input-files. wildcard OK
```