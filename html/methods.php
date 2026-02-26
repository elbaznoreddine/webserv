<?php
echo "METHOD: " . $_SERVER['REQUEST_METHOD'] . "\n";
echo "CONTENT_LENGTH: " . ($_SERVER['CONTENT_LENGTH'] ?? 'none') . "\n\n";


$method = $_SERVER['REQUEST_METHOD'];
if ($method === 'POST') {
    // Handle POST request data (e.g., from $_POST)
    echo "This was a POST request.".PHP_EOL;
    var_dump($_POST);
} elseif ($method === 'GET') {
    // Handle GET request data (e.g., from $_GET)
    echo "This was a GET request.".PHP_EOL;
    var_dump($_GET);
} else {
    // Handle other methods like PUT, DELETE, etc.
    echo "This was a [" . $method . "] request.".PHP_EOL;
}
?>