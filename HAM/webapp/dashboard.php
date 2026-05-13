<?php
declare(strict_types=1);

require_once __DIR__ . '/auth.php';

require_login();

$rows = db()->query('SELECT s.id, s.name, s.state, r.name AS room_name FROM switches s JOIN rooms r ON s.room_id = r.id ORDER BY r.name, s.name')->fetchAll();
$rooms = [];
foreach ($rows as $row) {
    $rooms[$row['room_name']][] = $row;
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
  <title>Dashboard</title>
  <link rel="stylesheet" href="assets/styles.css">
</head>
<body>
  <div class="layout">
    <div class="topbar">
      <div>
        <h1>Dashboard</h1>
        <div class="muted">Logged in as <?php echo h(current_username()); ?></div>
      </div>
      <div class="actions">
        <?php if (current_user_role() === 'admin'): ?>
          <a class="btn" href="admin.php">Admin</a>
        <?php endif; ?>
        <a class="btn" href="logout.php">Logout</a>
      </div>
    </div>

    <?php if (count($rooms) === 0): ?>
      <div class="card">
        <p class="muted">No switches yet. Ask admin to add rooms and switches.</p>
      </div>
    <?php else: ?>
      <?php foreach ($rooms as $roomName => $switches): ?>
        <div class="card">
          <h2><?php echo h($roomName); ?></h2>
          <table class="table">
            <thead>
              <tr>
                <th>Switch</th>
                <th>Status</th>
                <th>Actions</th>
              </tr>
            </thead>
            <tbody>
              <?php foreach ($switches as $sw): ?>
                <tr>
                  <td><?php echo h($sw['name']); ?></td>
                  <td>
                    <span class="status-pill <?php echo $sw['state'] ? 'status-on' : 'status-off'; ?>">
                      <?php echo $sw['state'] ? 'ON' : 'OFF'; ?>
                    </span>
                  </td>
                  <td>
                    <form method="post" action="switch_action.php" class="inline">
                      <input type="hidden" name="switch_id" value="<?php echo (int)$sw['id']; ?>">
                      <button class="btn" name="state" value="1" type="submit">ON</button>
                      <button class="btn" name="state" value="0" type="submit">OFF</button>
                    </form>
                  </td>
                </tr>
              <?php endforeach; ?>
            </tbody>
          </table>
        </div>
      <?php endforeach; ?>
    <?php endif; ?>
  </div>
</body>
</html>
