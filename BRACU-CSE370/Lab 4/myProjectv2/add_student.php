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
          <h1>Add New Student</h1>
          <form class="add_student_form" action="insert_student.php" method="post">
            Student ID: <input type="text" name="new_student_id"> 
			      Student Name: <input type="text" name="new_student_name"> 
			      Student Address: <input type="text" name="new_address"> 
			      Class: <input type="text" name="new_class">
            <input type="submit" value="Add Student" />
          </form>
        </div>
      </section>
    </main>
  </body>
</html>
