<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="stylesheet" href="css/style.css" />
    <link rel="preconnect" href="https://fonts.googleapis.com" />
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
    <link
      href="https://fonts.googleapis.com/css2?family=Fredoka:wght@300..700&display=swap"
      rel="stylesheet"
    />
    <title>XYZ School</title>
  </head>
  <body>
    <header>
      <nav>
        <div class="nav_logo">
          <h1><a href="index.php">XYZ SCHOOL</a></h1>
        </div>
        <ul class="nav_link">
          <li><a href="show_students.php">Students</a></li>
          <li><a href="#">Teachers</a></li>
        </ul>
      </nav>
    </header>
    <main>
      <section class="add_student">
        <div class="add_student_box">
          <h1>Update Student</h1>
          <?php
            require_once('DBconnect.php');

            if(isset($_GET['id'])){
                $student_id = $_GET['id'];
                $sql = "SELECT * FROM student WHERE student_id = '$student_id'";
                $result = mysqli_query($conn, $sql);

                if($row = mysqli_fetch_array($result)){
                    ?>
                    <form class="add_student_form" action="update_student.php" method="post">
                        <input type="hidden" name="student_id" value="<?php echo $row['student_id']; ?>">
                        <input type="text" name="new_student_name" value="<?php echo $row['student_name']; ?>" required>
                        <input type="text" name="new_address" value="<?php echo $row['address']; ?>" required>
                        <input type="text" name="new_class" value="<?php echo $row['class']; ?>" required>
                        <input type="submit" value="Update Student">
                    </form>
                    <?php
                } else {
                    echo "Student not found.";
                }
            } else {
                header("Location: admin_dashboard.php");
            }
          ?>
        </div>
      </section>
    </main>
  </body>
</html>


