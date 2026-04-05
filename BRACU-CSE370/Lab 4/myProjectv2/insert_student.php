<?php
require_once('DBconnect.php');

if(isset($_POST['new_student_id']) && isset($_POST['new_student_name']) && isset($_POST['new_address']) && isset($_POST['new_class'])){
	$student_id = $_POST['new_student_id'];
	$student_password = $_POST['new_student_name'];
	$address = $_POST['new_address'];
	$class = $_POST['new_class'];
	
	$sql = " INSERT INTO student VALUES( '$student_id', '$student_password', '$class', '$address' ) ";
	
	$result = mysqli_query($conn, $sql);
	echo "ran successfully";
	if(mysqli_affected_rows($conn)){
		//echo "Inserted Successfully";
		header("Location: admin_dashboard.php");
	}
	else{
		//echo "Insertion Failed";
		header("Location: add_student.php");
	}
}
?>