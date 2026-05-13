# Home Automation Web App

This folder contains a simple PHP + MySQL login system with two roles (admin, user).

## Setup

1) Copy this folder into your PHP server root or serve it from your local web server.
2) Edit `config.php` with your MySQL credentials.
3) Open `setup.php` once to create the database/tables and seed the admin user.
4) Log in at `login.php`.

## Pages

- `login.php` - Login page
- `admin.php` - Add rooms, switches, and users
- `dashboard.php` - User dashboard to change switch states
- `logout.php` - Sign out

## Notes

- The dashboard currently updates only the database state. It does not send live commands to hardware yet.
