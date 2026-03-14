#!/usr/bin/php-cgi
<?php

echo "Content-Type: text/html\r\n\r\n";

echo "<pre>";
echo "RAW INPUT:\n";
echo file_get_contents("php://input");
echo "\n\n_POST:\n";
print_r($_POST);
echo "\n_SERVER:\n";
print_r($_SERVER);
echo "</pre>";

?>