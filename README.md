# 3d-renderer-from-scratch
## Описание
Программный 3D рендерер с минимальным количеством зависимостей от сторонних библиотек.

## Зависимости
- SFML
- glm
- dear ImGui

## Сборка
- Microsoft Visual Studio 2019
- Самостоятельная установка зависимостей и сборка исходников (cmake не используется в проекте).

## Генерация UML диаграмм
1. Копируем .h файлы в отдельную папку.
2. ```hpp2plantuml -i "*.h" -o output.puml```   (https://github.com/thibaultmarin/hpp2plantuml)
3. ```java -jar plantuml.jar output.puml -tsvg``` (https://plantuml.com/)

## Галерея (базовый рендерер)

![alt text](screenshots/img.png)
![alt text](screenshots/img1.png)
![alt text](screenshots/img2.png)
![alt text](screenshots/img3.png)
![alt text](screenshots/img4.png)
![alt text](screenshots/img5.png)

## Галерея (продвинутый рендерер)

![alt text](screenshots/scene.png)
![alt text](screenshots/scene2.png)
![alt text](screenshots/scene3.png)
![alt text](screenshots/scene4.png)
![alt text](screenshots/scene5.png)
![alt text](screenshots/scene6.png)
![alt text](screenshots/scene7.png)
![alt text](screenshots/normal.png)
![alt text](screenshots/parallax.png)