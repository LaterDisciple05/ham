<?php
declare(strict_types=1);

require_once __DIR__ . '/auth.php';

if (is_logged_in()) {
    if (current_user_role() === 'admin') {
        header('Location: admin.php');
        exit;
    }
    header('Location: dashboard.php');
    exit;
}

$error = '';
$username = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = trim($_POST['username'] ?? '');
    $password = $_POST['password'] ?? '';

    if ($username === '' || $password === '') {
        $error = 'Enter username and password.';
    } else {
        $stmt = db()->prepare('SELECT id, username, password_hash, role FROM users WHERE username = ? LIMIT 1');
        $stmt->execute([$username]);
        $user = $stmt->fetch();

        if ($user && password_verify($password, $user['password_hash'])) {
            session_regenerate_id(true);
            $_SESSION['user_id'] = $user['id'];
            $_SESSION['username'] = $user['username'];
            $_SESSION['role'] = $user['role'];

            if ($user['role'] === 'admin') {
                header('Location: admin.php');
                exit;
            }

            header('Location: dashboard.php');
            exit;
        }

        $error = 'Invalid username or password.';
    }
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
  <title>Login</title>
  <link rel="stylesheet" href="assets/styles.css">
</head>
<body class="center">
  <div class="card small">
    <h1>Login</h1>
    <p class="muted">Use your account to continue.</p>
    <?php if ($error !== ''): ?>
      <div class="alert"><?php echo h($error); ?></div>
    <?php endif; ?>
    <form method="post" class="stack">
      <label>Username</label>
      <input type="text" name="username" value="<?php echo h($username); ?>" required>
      <label>Password</label>
      <input type="password" name="password" required>
      <button type="submit" class="btn primary">Login</button>
    </form>
  </div>
</body>
</html>
