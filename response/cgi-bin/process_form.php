<!DOCTYPE html>
<html>
<head>
    <title>Form Submission Result</title>
</head>
<body>
    <h1> Form Submission Result </h1> 
    <?php
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        // Check if the form has been submitted
        // echo $_POST['name'];
        // echo $_POST['age'];
        if (isset($_POST['name']) && isset($_POST['age'])) {
            // echo '++++++++++++++++++++++++++++++++++';
            // Retrieve the submitted name and age
            $name = $_POST['name'];
            $age = $_POST['age'];

            // Display the submitted values
            echo '<p>The submitted name is: ' . $name . '</p>';
            echo '<p>The submitted age is: ' . $age . '</p>';
        }
    }
    ?>
</body>
</html>