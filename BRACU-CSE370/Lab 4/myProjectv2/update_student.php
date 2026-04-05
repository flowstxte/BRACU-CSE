<?php
require_once('DBconnect.php');

if(isset($_POST['student_id'])){
    $student_id = $_POST['student_id'];
    $student_name = $_POST['new_student_name'];
    $address = $_POST['new_address'];
    $class = $_POST['new_class'];

    $sql = "UPDATE student SET student_name = '$student_name', address = '$address', class = '$class' WHERE student_id = '$student_id'";

    if(mysqli_query($conn, $sql)){
        header("Location: admin_dashboard.php");
    } else {
        echo "Error updating student: " . mysqli_error($conn);
    }
}

