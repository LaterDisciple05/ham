<?php
declare(strict_types=1);

require_once __DIR__ . '/auth.php';

require_login();

$switchId = (int)($_POST['switch_id'] ?? 0);
$stateValue = $_POST['state'] ?? null;

if ($switchId < 1 || ($stateValue !== '1' && $stateValue !== '0')) {
    header('Location: dashboard.php');
    exit;
}

$state = $stateValue === '1' ? 1 : 0;
$stmt = db()->prepare('UPDATE switches SET state = ? WHERE id = ?');
$stmt->execute([$state, $switchId]);

header('Location: dashboard.php');
exit;
