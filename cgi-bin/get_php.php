#!/usr/bin/php-cgi
<?php

echo "Content-Type: text/html\r\n\r\n";

echo "<html><body>";
echo "<h1>PHP CGI GET Test</h1>";

echo "<h2>Query Parameters</h2>";
echo "\n\n_GET:\n";
print_r($_GET);
foreach ($_GET as $key => $value) {
    echo "<p>$key = $value</p>";
}

echo "</body></html>";
?>