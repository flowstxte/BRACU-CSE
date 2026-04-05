<?php
require_once('DBconnect.php');

if(isset($_GET['id'])) {
    $student_id = $_GET['id'];
    $sql = "DELETE FROM student WHERE student_id = '$student_id'";
    $result = mysqli_query($conn, $sql);

    if(mysqli_affected_rows($conn) > 0) {
        header("Location: admin_dashboard.php");
    } else {
        echo "Error deleting student: " . mysqli_error($conn);
    }
}
?>
