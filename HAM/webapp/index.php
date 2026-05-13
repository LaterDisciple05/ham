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

header('Location: login.php');
exit;
