# Operation_System_Lab_2
Лабораторная работа по предмету Операционные системы

### Вариант: procfs.
### Структуры pci_dev, inode. 

### Задание 
Разработать комплекс программ на пользовательском уровне и уровне ярда, который собирает информацию на стороне ядра и передает информацию на уровень пользователя, и выводит ее в удобном для чтения человеком виде. Программа на уровне пользователя получает на вход аргумент(ы) командной строки (не адрес!), позволяющие идентифицировать из системных таблиц необходимый путь до целевой структуры, осуществляет передачу на уровень ядра, получает информацию из данной структуры и распечатывает структуру в стандартный вывод. Загружаемый модуль ядра принимает запрос через указанный в задании интерфейс, определяет путь до целевой структуры по переданному запросу и возвращает результат на уровень пользователя.

Вопросы для защиты:
1. Алгоритмы планирования 
2. copy_to_user, copy_from_user - что делает? Зачем использовать?
3. Memory Management Unit. 
