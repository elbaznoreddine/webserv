<?php
// CLI-compatible way
$query = getenv('QUERY_STRING') ?: '';
parse_str($query, $params);

$name = $params['name'] ?? 'Guest';
$age  = $params['age'] ?? 'unknown';

echo "Name: $name\n";
echo "Age: $age\n";
?>