# Web Crawler

This is web crawler build with [Chilkat's CkSpider](https://www.chilkatsoft.com/) and [Gumbo Parser](https://github.com/google/gumbo-parser). The program will collect a given number of web pages anb build an index for information retrieval over that collection. The default number of pages the crawler will try to visit before halting is 100000 (one hundred thousand), then it will create an index for 5000 (five thousand) of those, you can change this values by modifying the ```PAGES_TO_COLLECT``` and ```PAGES_TO_INDEX``` constants located in ```main.cpp```.

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
* ```-b [COLLECTION_PATH - optional]``` replacing ```[COLLECTION_PATH]``` with the path where your html collection is stored or simply leaving it blank, by default, the collection path will be ```output/html/```.

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

**Important:** your collection folder containing the html files for indexing should contain files named ```1.html```, ```2.html```... ```n.html``` or else the crawler won't locate them.