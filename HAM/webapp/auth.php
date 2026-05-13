<?php
declare(strict_types=1);

require_once __DIR__ . '/config.php';
require_once __DIR__ . '/db.php';

function is_logged_in(): bool
{
    return !empty($_SESSION['user_id']);
}

function current_user_role(): string
{
    return $_SESSION['role'] ?? '';
}

function current_username(): string
{
    return $_SESSION['username'] ?? '';
}

function require_login(): void
{
    if (!is_logged_in()) {
        header('Location: login.php');
        exit;
    }
}

function require_admin(): void
{
    require_login();
    if (current_user_role() !== 'admin') {
        header('Location: dashboard.php');
        exit;
    }
}
