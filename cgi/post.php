<?php
// Read from stdin (CLI compatible)
$stdin = fopen('php://stdin', 'r');
$post_body = stream_get_contents($stdin);

// Parse key=value pairs
parse_str($post_body, $params);

// Fallbacks
$name = $params['name'] ?? 'Guest';
$age  = $params['age'] ?? 'unknown';

echo "Received via POST (stdin):\n";
echo "Name: $name\n";
echo "Age: $age\n";
?>