"use strict";

/* exported handleRequest */

function handleRequest(request, response) {
  response.setHeader("Content-Type", "text/plain", false);

  let headers = {};
  // Why on earth is this a nsISimpleEnumerator...
  let enumerator = request.headers;
  while (enumerator.hasMoreElements()) {
    let header = enumerator.getNext().data;
    headers[header.toLowerCase()] = request.getHeader(header);
  }

  response.write(JSON.stringify(headers));
}

