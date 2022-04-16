"use strict";

function handleRequest(request, response) {
  response.setHeader("Content-Type", "text/plain; charset=UTF-8", false);

  if (request.hasHeader("pragma") && request.hasHeader("cache-control")) {
    response.write(`${request.getHeader("pragma")}:${request.getHeader("cache-control")}`);
  }
}