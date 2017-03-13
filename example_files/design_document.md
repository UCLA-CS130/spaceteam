# Spaceteam Design Document

---

## Markdown Rendering

### Description
When accessing a file with .md extension, the static request handler transparently converts it to HTML using an external markdown library.

### High-Level Implementation
* Added cpp-markdown library to our codebase, as recommended in the [Stack Overflow link]((http://stackoverflow.com/questions/889434/markdown-implementations-for-c-c)) sent by the professors.
* Modified the static handler in that if it finds a file with “.md”, it will convert it to html using the cpp-markdown library and then writes the results in the outgoing string stream for the response.

### URL for demoing this feature:
* This current document is in Markdown format!
* This is the [original md file](https://github.com/UCLA-CS130/spaceteam/blob/master/example_files/mdtest.md) in Github.
* This is our [test Markdown file](/static/mdtest.md) in our server that converts the MD file into html.

### Exact Walkthrough for the instructors to follow along
1. Go to [THIS LINK](https://github.com/UCLA-CS130/spaceteam/blob/master/example_files/mdtest.md) to see the original md file in our example_files folder, hosted by our server.
2. Select [THIS LINK](/static/mdtest.md) to see the resulting processed HTML file.

---

## S3 Handler

### Description
Request handler that serves text files directly from an AWS S3 bucket.

### High-Level Implementation
* Use system call to node.js script which downloads the S3 object and outputs the object’s data.
* Read output of node.js script in S3 Handler and set response body

### URL for demoing this feature
* This is our [link to server](/s3/test.html)
* This is our [direct link to same file on S3](https://s3-us-west-1.amazonaws.com/cs130-spaceteam/test.html) for comparison. 

### Exact Walkthrough for the instructors to follow along
1. Check out the [direct link on S3](https://s3-us-west-1.amazonaws.com/cs130-spaceteam/test.html) for the original file.
2. Check out [our server's hosted version](/s3/test.html) of the S3 file.

