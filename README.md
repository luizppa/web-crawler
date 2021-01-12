# Sample Web Crawler

A sample application that crawls web pages using [Chilkat](https://www.chilkatsoft.com/).

This is an example on how to use Chilkat's CkSpider to crawl web pages. The output is a list of pages visited from a given starting point, the total time spent crawling, and the average time per page and the html content of each page. The default number of pages the crawler will try to visit before halting is 100000 (one hudred thousand), you can change this by modifying the constant ```PAGES_TO_COLLECT``` located in ```main.cpp```.

## Installing and Running

[Installing Chilkat for cpp](https://www.chilkatsoft.com/downloads_CPP.asp)

After installing Chilkat, you can build the project with ```make```, which creates the executable file ```build/web-crawler```. To run the application, you can either execute ```make run``` (to run with sample inputs) or use ```./build/web-crawler [PATH_TO_SEED_FILE]``` replacing ```[PATH_TO_SEED_FILE]``` with the path to the file containing your seeds. Your seed file should be a list of urls where the crawler will start visiting separated with line breaks like:

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