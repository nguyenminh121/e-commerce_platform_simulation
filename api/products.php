<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
header('Access-Control-Allow-Headers: Content-Type');

// Mock data - in real app, this would come from database
$products = [
    [
        'id' => 1,
        'name' => 'iPhone 15 Pro Max 256GB',
        'price' => 28990000,
        'stock' => 50,
        'category' => 'Electronics',
        'description' => 'Latest iPhone with Pro camera system'
    ],
    [
        'id' => 2,
        'name' => 'Laptop Dell XPS 13 2024',
        'price' => 22490000,
        'stock' => 25,
        'category' => 'Electronics',
        'description' => 'Ultra-thin laptop with premium build'
    ],
    [
        'id' => 3,
        'name' => 'Tai nghe Sony WH-1000XM5',
        'price' => 5990000,
        'stock' => 100,
        'category' => 'Electronics',
        'description' => 'Premium noise-cancelling headphones'
    ],
    [
        'id' => 4,
        'name' => 'Apple Watch Series 9',
        'price' => 8490000,
        'stock' => 75,
        'category' => 'Electronics',
        'description' => 'Latest smartwatch with health features'
    ],
    [
        'id' => 5,
        'name' => 'iPad Air M2 256GB',
        'price' => 15990000,
        'stock' => 30,
        'category' => 'Electronics',
        'description' => 'Powerful tablet for work and creativity'
    ]
];

$method = $_SERVER['REQUEST_METHOD'];

switch($method) {
    case 'GET':
        if(isset($_GET['id'])) {
            $id = (int)$_GET['id'];
            $product = null;
            foreach($products as $p) {
                if($p['id'] == $id) {
                    $product = $p;
                    break;
                }
            }
            if($product) {
                echo json_encode(['status' => 'success', 'data' => $product]);
            } else {
                http_response_code(404);
                echo json_encode(['status' => 'error', 'message' => 'Product not found']);
            }
        } else {
            echo json_encode(['status' => 'success', 'data' => $products]);
        }
        break;
        
    case 'POST':
        $input = json_decode(file_get_contents('php://input'), true);
        if($input && isset($input['name']) && isset($input['price'])) {
            $newProduct = [
                'id' => count($products) + 1,
                'name' => $input['name'],
                'price' => (float)$input['price'],
                'stock' => isset($input['stock']) ? (int)$input['stock'] : 0,
                'category' => isset($input['category']) ? $input['category'] : 'General',
                'description' => isset($input['description']) ? $input['description'] : ''
            ];
            echo json_encode(['status' => 'success', 'message' => 'Product created', 'data' => $newProduct]);
        } else {
            http_response_code(400);
            echo json_encode(['status' => 'error', 'message' => 'Invalid input']);
        }
        break;
        
    default:
        http_response_code(405);
        echo json_encode(['status' => 'error', 'message' => 'Method not allowed']);
        break;
}
?>