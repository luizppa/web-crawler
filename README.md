# Web Crawler

This is web crawler build with [Chilkat's CkSpider](https://www.chilkatsoft.com/) and [Gumbo Parser](https://github.com/google/gumbo-parser). The program will collect a given number of web pages anb build an index for information retrieval over that collection. The default number of pages the crawler will try to visit before halting is 100000 (one hudred thousand), you can change this by modifying the constant ```PAGES_TO_COLLECT``` located in ```main.cpp```.

## Installing and Running

[Installing Chilkat for cpp](https://www.chilkatsoft.com/downloads_CPP.asp)

After installing Chilkat: 

```
$ sudo make install
$ make
```

This will install [Gumbo Parser](https://github.com/google/gumbo-parser) and build the project, creating an executable file within ```build/```. To run the application, you can either run ```make run``` (to run with sample inputs) or use ```./build/web-crawler [PATH_TO_SEED_FILE]``` replacing ```[PATH_TO_SEED_FILE]``` with the path to the file containing your seeds. Your seed file should be a list of urls where the crawler will start visiting separated with line breaks like:

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