<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="stylesheet" href="css/style.css" />
    <link rel="preconnect" href="https://fonts.googleapis.com" />
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
    <link href="https://fonts.googleapis.com/css2?family=Fredoka:wght@300..700&display=swap" rel="stylesheet" />
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
        <section class="students">
            <div class="student_box">
                <h1>Students List</h1>
                <table class="student_table">
                    <thead>
                        <tr>
                            <th>Student ID</th>
                            <th>Student Name</th>
                            <th>Student Address</th>
                            <th>Class</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php 
                        require_once("DBconnect.php");
                        $sql = "SELECT * FROM student";
                        $result = mysqli_query($conn, $sql);
                        if(mysqli_num_rows($result) > 0){
                            while($row = mysqli_fetch_assoc($result)){
                        ?>
                        <tr>
                            <td><?php echo $row["student_id"]; ?></td>
                            <td><?php echo $row["student_name"]; ?></td>
                            <td><?php echo $row["address"]; ?></td>
                            <td><?php echo $row["class"]; ?></td>
                            <td style="background-color:rgba(255, 255, 255, 0.2);">
                                <a style="text-decoration:none;" href="modify_student.php?id=<?php echo $row['student_id']; ?>">
                                    <img src="img/update.png" alt="update" width="20" height="20"/>
                                </a>
                                <a style="text-decoration:none;" href="delete_student.php?id=<?php echo $row['student_id']; ?>" onclick="return confirm('Confirm deletion?');">
                                    <img src="img/delete.png" alt="delete" width="20" height="20"/>
                                </a>
                            </td>
                        </tr>
                        <?php
                            }
                        }
                        ?>
                    </tbody>
                </table>
                <a href="add_student.php" class="btn">Add Student</a>

            </div>
        </section>
    </main>
</body>
</html>
