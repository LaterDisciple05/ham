<?php
declare(strict_types=1);

require_once __DIR__ . '/config.php';
require_once __DIR__ . '/db.php';

$messages = [];
$errors = [];

try {
    $server = db_server();
    $server->exec('CREATE DATABASE IF NOT EXISTS `' . DB_NAME . '` CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci');
    $messages[] = 'Database checked.';

    $pdo = db();
    $pdo->exec(
        'CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            username VARCHAR(50) NOT NULL UNIQUE,
            password_hash VARCHAR(255) NOT NULL,
            role ENUM("admin", "user") NOT NULL DEFAULT "user",
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        ) ENGINE=InnoDB'
    );

    $pdo->exec(
        'CREATE TABLE IF NOT EXISTS rooms (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(80) NOT NULL UNIQUE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        ) ENGINE=InnoDB'
    );

    $pdo->exec(
        'CREATE TABLE IF NOT EXISTS switches (
            id INT AUTO_INCREMENT PRIMARY KEY,
            room_id INT NOT NULL,
            name VARCHAR(80) NOT NULL,
            state TINYINT(1) NOT NULL DEFAULT 0,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (room_id) REFERENCES rooms(id) ON DELETE CASCADE
        ) ENGINE=InnoDB'
    );

    $count = (int)$pdo->query("SELECT COUNT(*) FROM users WHERE role = 'admin'")->fetchColumn();
    if ($count === 0) {
        $hash = password_hash(ADMIN_SEED_PASS, PASSWORD_DEFAULT);
        $stmt = $pdo->prepare('INSERT INTO users (username, password_hash, role) VALUES (?, ?, "admin")');
        $stmt->execute([ADMIN_SEED_USER, $hash]);
        $messages[] = 'Admin user created.';
    } else {
        $messages[] = 'Admin user already exists.';
    }

    if (ADMIN_SEED_PASS === '') {
        $messages[] = 'Admin password is empty. Update it after setup.';
    }

    $messages[] = 'Setup complete.';
} catch (Throwable $e) {
    $errors[] = $e->getMessage();
}

function h(string $value): string
{
    return htmlspecialchars($value, ENT_QUOTES, 'UTF-8');
}
?>
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Setup</title>
  <link rel="stylesheet" href="assets/styles.css">
</head>
<body class="center">
  <div class="card small">
    <h1>Setup</h1>
    <?php foreach ($errors as $err): ?>
      <div class="alert danger"><?php echo h($err); ?></div>
    <?php endforeach; ?>
    <?php foreach ($messages as $msg): ?>
      <div class="alert"><?php echo h($msg); ?></div>
    <?php endforeach; ?>
    <div class="actions">
      <a class="btn" href="login.php">Go to login</a>
    </div>
  </div>
</body>
</html>
