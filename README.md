# OS-SHELL

1/ test claim of zombie:
- 1st terminal:
    whoami
    sleep 1 & sleep 2 & sleep 3 &

- 2nd terminal:
    ps -elfH | grep root
