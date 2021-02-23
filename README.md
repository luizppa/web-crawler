# Web Crawler

This is a web crawler built with [Chilkat's CkSpider](https://www.chilkatsoft.com/) and [Gumbo Parser](https://github.com/google/gumbo-parser). The program will collect a given number of web pages anb build an index for information retrieval over that collection. The default number of pages the crawler will try to visit before halting is 100000 (one hundred thousand), then it will create an index for those. You can change this value by modifying the ```PAGES_TO_COLLECT``` constant located in ```main.cpp```.

* [Installing](#installing)
* [Usage](#usage)
* [Example](#example)

## Installing

[Installing Chilkat for cpp](https://www.chilkatsoft.com/downloads_CPP.asp)

After installing Chilkat: 

```
$ sudo make install
$ make
```

This will install [Gumbo Parser](https://github.com/google/gumbo-parser) and build the project (you may need to run ```sudo ldconfig``` afterwards), creating an executable file within ```build/```.

## Usage

To run the application, you can either use ```make run``` (to run with sample inputs, this will automaticaly crawl for some pages with a predefined seed at ```input/seed``` and build the index for a few of those pages) or use ```./build/web-crawler``` with custom options.

The available options are:

* ```-c [SEED_FILE]``` replacing ```[SEED_FILE]``` with the path to the file containing your seeds, see [examples](#example).
* ```-b [COLLECTION_PATH - optional]``` replacing ```[COLLECTION_PATH]``` with the path where your html collection is stored or simply leaving it blank, by default the collection path will be ```output/collection.jl```.

The documents on the collection are indexed in batches, by default, the maximum batch size is 4096, defined in ```include/indexer.hpp```. If the batch size is too big, the application will consume a large amount of RAM, however, if it is overly small, the execution time and disk usage may increase.

The output will be a ```collection.jl``` file, which is going to be the collected documents as list of JSON objects separated by line breaks, and a ```index.idx``` file, which will be the inverted index for that collection. The JSON for the documents is an object with two keys: _url_ and _html\_content_, containing the link of the document on the web and HTML content of the document respectively. Each line of the inverted index representes a term following this format:

_term n d<sub>1</sub> n<sub>d1</sub> p<sub>1,d1</sub> p<sub>2,d1</sub> p<sub>ndn,dn</sub>_

Where term is the indexed word, n is the number of documents where the term is present,  d<sub>i</sub> is the i-th document where the term is present, n<sub>di</sub> is the number of times the term appears in d<sub>i</sub> and p<sub>j,di</sub> is the position of the j-th occurrence of the term in d<sub>i</sub>.

> *Be cautious when modifying the maximum batch size as it will require a lot of RAM, e.g. indexing 60000 documents at once can consume over 5GB. Also, make sure you have enough storage space.*

## Example

Your seed file should be a list of urls where the crawler will start visiting separated with line breaks like:

```
ufmg.br
kurzgesagt.org
www.cam.ac.uk
www.nasa.gov
github.com
medium.com
www.cnnbrasil.com.br
disney.com.br
en.wikipedia.org
```
