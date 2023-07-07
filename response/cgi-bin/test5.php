<!DOCTYPE html>
<html>
<head>
    <title>GET Form Example</title>
</head>
<body>
    <form action="process_form.php" method="GET">
        <label for="name">Name:</label>
        <input type="text" name="name" id="name" required><br><br>

        <label for="age">Age:</label>
        <input type="number" name="age" id="age" required><br><br>

        <input type="submit" value="Submit">
    </form>
</body>
</html>