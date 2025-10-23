<?php
// Bridge between C++ and Web Interface

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['action'])) {
    
    switch($_POST['action']) {
        case 'run_cpp_simulation':
            $output = runCppSimulation();
            echo json_encode([
                'status' => 'success',
                'output' => $output,
                'timestamp' => date('Y-m-d H:i:s')
            ]);
            break;
            
        case 'get_cpp_data':
            $data = getCppSimulationData();
            echo json_encode([
                'status' => 'success', 
                'data' => $data
            ]);
            break;
            
        default:
            echo json_encode(['status' => 'error', 'message' => 'Invalid action']);
    }
    
} else {
    // Show interface
    showInterface();
}

function runCppSimulation() {
    $cppPath = __DIR__ . '/iShopefy.exe';
    
    if (!file_exists($cppPath)) {
        return "Error: iShopefy.exe not found. Please compile first.";
    }
    
    // Execute C++ program and capture output
    ob_start();
    $result = shell_exec($cppPath . ' 2>&1');
    $output = ob_get_clean();
    
    return $result ?: "Program executed but no output captured.";
}

function getCppSimulationData() {
    // Mock data that would come from C++ program
    return [
        'cart_items' => [
            ['name' => 'Wireless Mouse', 'quantity' => 2, 'price' => 12.50],
            ['name' => 'Mechanical Keyboard', 'quantity' => 1, 'price' => 45.99],
            ['name' => 'USB-C Cable', 'quantity' => 2, 'price' => 5.49]
        ],
        'subtotal' => 81.97,
        'discount' => 8.20,
        'total' => 73.77,
        'discount_type' => 'Student Discount',
        'payment_method' => 'Credit Card'
    ];
}

function showInterface() {
?>
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>iShopefy - C++ Integration</title>
    <link rel="stylesheet" href="styles/index.css">
    <style>
        .cpp-integration {
            background: white;
            margin: 20px 0;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
        }
        .cpp-output {
            background: #1e1e1e;
            color: #d4d4d4;
            padding: 20px;
            border-radius: 8px;
            font-family: 'Courier New', monospace;
            font-size: 14px;
            line-height: 1.4;
            white-space: pre-wrap;
            margin: 15px 0;
            max-height: 400px;
            overflow-y: auto;
        }
        .btn-cpp {
            background: linear-gradient(45deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            padding: 12px 25px;
            border-radius: 25px;
            font-weight: bold;
            cursor: pointer;
            margin: 10px 5px;
            transition: transform 0.2s;
        }
        .btn-cpp:hover {
            transform: translateY(-2px);
        }
        .integration-stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }
        .stat-card {
            background: linear-gradient(135deg, #ee4d2d, #ff6b35);
            color: white;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        .stat-value {
            font-size: 24px;
            font-weight: bold;
            margin-bottom: 5px;
        }
        .loading {
            display: none;
            text-align: center;
            margin: 20px 0;
        }
        .spinner {
            border: 3px solid #f3f3f3;
            border-radius: 50%;
            border-top: 3px solid #ee4d2d;
            width: 30px;
            height: 30px;
            animation: spin 1s linear infinite;
            margin: 0 auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <?php include 'includes/header.php'; ?>
    
    <div class="container">
        <section class="cpp-integration">
            <h2><i class="fas fa-code"></i> C++ Integration Console</h2>
            <p>Kết nối giữa website PHP và chương trình C++ OOP simulation.</p>
            
            <div class="integration-stats">
                <div class="stat-card">
                    <div class="stat-value">OOP</div>
                    <div>C++ Classes</div>
                </div>
                <div class="stat-card">
                    <div class="stat-value">API</div>
                    <div>PHP Bridge</div>
                </div>
                <div class="stat-card">
                    <div class="stat-value">JSON</div>
                    <div>Data Exchange</div>
                </div>
                <div class="stat-card">
                    <div class="stat-value">Real-time</div>
                    <div>Execution</div>
                </div>
            </div>
            
            <div class="controls">
                <button class="btn-cpp" onclick="runSimulation()">
                    <i class="fas fa-play"></i> Run C++ Simulation
                </button>
                <button class="btn-cpp" onclick="getCppData()">
                    <i class="fas fa-database"></i> Get Simulation Data
                </button>
                <button class="btn-cpp" onclick="clearOutput()">
                    <i class="fas fa-trash"></i> Clear Output
                </button>
            </div>
            
            <div class="loading">
                <div class="spinner"></div>
                <p>Executing C++ program...</p>
            </div>
            
            <div id="output-container">
                <h3><i class="fas fa-terminal"></i> Console Output:</h3>
                <div class="cpp-output" id="cpp-output">
Click "Run C++ Simulation" to execute the program...
                </div>
            </div>
        </section>
    </div>
    
    <?php include 'includes/footer.php'; ?>
    
    <script>
        async function runSimulation() {
            const loading = document.querySelector('.loading');
            const output = document.getElementById('cpp-output');
            
            loading.style.display = 'block';
            output.textContent = 'Executing C++ program...';
            
            try {
                const response = await fetch('cpp_bridge.php', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'action=run_cpp_simulation'
                });
                
                const result = await response.json();
                
                if(result.status === 'success') {
                    output.textContent = result.output;
                    showNotification('C++ simulation completed successfully!', 'success');
                } else {
                    output.textContent = 'Error: ' + (result.message || 'Unknown error');
                    showNotification('Failed to run simulation', 'error');
                }
            } catch(error) {
                output.textContent = 'Network Error: ' + error.message;
                showNotification('Network error occurred', 'error');
            } finally {
                loading.style.display = 'none';
            }
        }
        
        async function getCppData() {
            const output = document.getElementById('cpp-output');
            
            try {
                const response = await fetch('cpp_bridge.php', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'action=get_cpp_data'
                });
                
                const result = await response.json();
                
                if(result.status === 'success') {
                    output.textContent = JSON.stringify(result.data, null, 2);
                    showNotification('Simulation data loaded!', 'success');
                }
            } catch(error) {
                output.textContent = 'Error loading data: ' + error.message;
            }
        }
        
        function clearOutput() {
            document.getElementById('cpp-output').textContent = 'Output cleared.';
        }
        
        function showNotification(message, type = 'success') {
            // Use the notification system from main.js if available
            if(typeof window.iShopefy !== 'undefined' && window.iShopefy.showNotification) {
                window.iShopefy.showNotification(message, type);
            } else {
                alert(message);
            }
        }
    </script>
</body>
</html>
<?php
}
?>