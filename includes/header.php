<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>iShopefy - Nền tảng mua sắm trực tuyến</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" integrity="sha512-iecdLmaskl7CVkqkXNQ/ZH/XkYDH5dQ8YYcElQSvqcyVLLD9aMhXd13uQjoXtEKNosOWaZqXgel0g==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet" href="styles/index.css">
</head>
<body>
    <!-- Top Navigation Bar -->
    <div class="top-nav">
        <div class="container">
            <div class="top-nav-left">
                <a href="#"><i class="fas fa-bell"></i> Thông báo</a>
                <a href="#"><i class="fas fa-question-circle"></i> Hỗ trợ</a>
            </div>
            <div class="top-nav-right">
                <a href="#"><i class="fas fa-globe"></i> Tiếng Việt</a>
                <a href="user/profile.php"><i class="fas fa-user"></i> Đăng nhập</a>
                <a href="#"><i class="fas fa-user-plus"></i> Đăng ký</a>
            </div>
        </div>
    </div>

    <!-- Main Header -->
    <header class="main-header">
        <div class="container">
            <div class="header-content">
                <div class="logo">
                    <a href="index.php">
                        <i class="fas fa-shopping-bag"></i>
                        <span>iShopefy</span>
                    </a>
                </div>
                
                <div class="search-bar">
                    <input type="text" placeholder="Tìm kiếm sản phẩm, thương hiệu...">
                    <button type="submit"><i class="fas fa-search"></i></button>
                </div>

                <div class="header-actions">
                    <a href="cart.php" class="cart-icon">
                        <i class="fas fa-shopping-cart"></i>
                        <span class="cart-badge">0</span>
                    </a>
                </div>
            </div>

            <nav class="main-nav">
                <ul>
                    <li><a href="index.php"><i class="fas fa-home"></i> Trang chủ</a></li>
                    <li><a href="products.php"><i class="fas fa-th"></i> Sản phẩm</a></li>
                    <li><a href="#"><i class="fas fa-fire"></i> Flash Sale</a></li>
                    <li><a href="#"><i class="fas fa-tags"></i> Khuyến mãi</a></li>
                    <li><a href="order.php"><i class="fas fa-box"></i> Đơn hàng</a></li>
                </ul>
            </nav>
        </div>
    </header>