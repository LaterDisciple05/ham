<?php
declare(strict_types=1);

require_once __DIR__ . '/auth.php';

require_admin();

$message = '';
$error = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $action = $_POST['action'] ?? '';

    try {
        if ($action === 'add_room') {
            $name = trim($_POST['room_name'] ?? '');
            if ($name === '') {
                $error = 'Room name is required.';
            } else {
                $stmt = db()->prepare('INSERT INTO rooms (name) VALUES (?)');
                $stmt->execute([$name]);
                $message = 'Room added.';
            }
        }

        if ($action === 'add_switch') {
            $name = trim($_POST['switch_name'] ?? '');
            $roomId = (int)($_POST['room_id'] ?? 0);
            if ($name === '' || $roomId < 1) {
                $error = 'Switch name and room are required.';
            } else {
                $stmt = db()->prepare('INSERT INTO switches (room_id, name) VALUES (?, ?)');
                $stmt->execute([$roomId, $name]);
                $message = 'Switch added.';
            }
        }

        if ($action === 'add_user') {
            $username = trim($_POST['user_name'] ?? '');
            $password = $_POST['user_password'] ?? '';
            $role = $_POST['user_role'] ?? 'user';
            if ($role !== 'admin' && $role !== 'user') {
                $role = 'user';
            }
            if ($username === '' || $password === '') {
                $error = 'Username and password are required.';
            } else {
                $hash = password_hash($password, PASSWORD_DEFAULT);
                $stmt = db()->prepare('INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)');
                $stmt->execute([$username, $hash, $role]);
                $message = 'User added.';
            }
        }
    } catch (Throwable $e) {
        $error = 'Operation failed. Check for duplicate names.';
    }
}

$rooms = db()->query('SELECT id, name FROM rooms ORDER BY name')->fetchAll();
$switches = db()->query('SELECT s.id, s.name, s.state, r.name AS room_name FROM switches s JOIN rooms r ON s.room_id = r.id ORDER BY r.name, s.name')->fetchAll();
$users = db()->query('SELECT id, username, role, created_at FROM users ORDER BY username')->fetchAll();

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
  <title>Admin</title>
  <link rel="stylesheet" href="assets/styles.css">
</head>
<body>
  <div class="layout">
    <div class="topbar">
      <div>
        <h1>Admin</h1>
        <div class="muted">Logged in as <?php echo h(current_username()); ?></div>
      </div>
      <div class="actions">
        <a class="btn" href="dashboard.php">Dashboard</a>
        <a class="btn" href="logout.php">Logout</a>
      </div>
    </div>

    <?php if ($error !== ''): ?>
      <div class="alert danger"><?php echo h($error); ?></div>
    <?php elseif ($message !== ''): ?>
      <div class="alert"><?php echo h($message); ?></div>
    <?php endif; ?>

    <div class="card">
      <h2>Add room</h2>
      <form method="post" class="form-grid">
        <input type="hidden" name="action" value="add_room">
        <div>
          <label>Room name</label>
          <input type="text" name="room_name" placeholder="e.g. Living Room" required>
        </div>
        <div class="full"></div>
        <button class="btn primary" type="submit">Add room</button>
      </form>
    </div>

    <div class="card">
      <h2>Add switch</h2>
      <?php if (count($rooms) === 0): ?>
        <p class="muted">Add a room first.</p>
      <?php else: ?>
        <form method="post" class="form-grid">
          <input type="hidden" name="action" value="add_switch">
          <div>
            <label>Switch name</label>
            <input type="text" name="switch_name" placeholder="e.g. Main Light" required>
          </div>
          <div>
            <label>Room</label>
            <select name="room_id" required>
              <?php foreach ($rooms as $room): ?>
                <option value="<?php echo (int)$room['id']; ?>"><?php echo h($room['name']); ?></option>
              <?php endforeach; ?>
            </select>
          </div>
          <button class="btn primary" type="submit">Add switch</button>
        </form>
      <?php endif; ?>
    </div>

    <div class="card">
      <h2>Add user</h2>
      <form method="post" class="form-grid">
        <input type="hidden" name="action" value="add_user">
        <div>
          <label>Username</label>
          <input type="text" name="user_name" required>
        </div>
        <div>
          <label>Password</label>
          <input type="password" name="user_password" required>
        </div>
        <div>
          <label>Role</label>
          <select name="user_role">
            <option value="user">User</option>
            <option value="admin">Admin</option>
          </select>
        </div>
        <button class="btn primary" type="submit">Add user</button>
      </form>
    </div>

    <div class="card">
      <h2>Rooms</h2>
      <?php if (count($rooms) === 0): ?>
        <p class="muted">No rooms yet.</p>
      <?php else: ?>
        <table class="table">
          <thead>
            <tr>
              <th>Room</th>
            </tr>
          </thead>
          <tbody>
            <?php foreach ($rooms as $room): ?>
              <tr>
                <td><?php echo h($room['name']); ?></td>
              </tr>
            <?php endforeach; ?>
          </tbody>
        </table>
      <?php endif; ?>
    </div>

    <div class="card">
      <h2>Switches</h2>
      <?php if (count($switches) === 0): ?>
        <p class="muted">No switches yet.</p>
      <?php else: ?>
        <table class="table">
          <thead>
            <tr>
              <th>Room</th>
              <th>Switch</th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            <?php foreach ($switches as $sw): ?>
              <tr>
                <td><?php echo h($sw['room_name']); ?></td>
                <td><?php echo h($sw['name']); ?></td>
                <td>
                  <span class="status-pill <?php echo $sw['state'] ? 'status-on' : 'status-off'; ?>">
                    <?php echo $sw['state'] ? 'ON' : 'OFF'; ?>
                  </span>
                </td>
              </tr>
            <?php endforeach; ?>
          </tbody>
        </table>
      <?php endif; ?>
    </div>

    <div class="card">
      <h2>Users</h2>
      <table class="table">
        <thead>
          <tr>
            <th>Username</th>
            <th>Role</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach ($users as $user): ?>
            <tr>
              <td><?php echo h($user['username']); ?></td>
              <td><?php echo h($user['role']); ?></td>
            </tr>
          <?php endforeach; ?>
        </tbody>
      </table>
    </div>
  </div>
</body>
</html>
