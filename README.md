# Sample Web Crawler

A sample application that crawls web pages using [Chilkat](https://www.chilkatsoft.com/).

This is an example on how to use Chilkat's CkSpider to crawl web pages. The output is a list of pages visited from a given starting point, the total time spent crawling, and the average time per page.

## Installing and Running

[Installing Chilkat for cpp](https://www.chilkatsoft.com/downloads_CPP.asp)

After installing Chilkat, you can build the project with ```make```, which creates the executable file ```build/ir_tp1```. To run the application, you can either execute ```make run``` (to run with sample inputs) or use ```./build/ir_tp1 [STARTING_URL] [PAGES_TO_CRAWL]``` replacing [STARTING_URL] with the starting url address where the crawler should be initialized and [PAGES_TO_CRAWL] with the number of pages the crawler should visit from the starting point.
