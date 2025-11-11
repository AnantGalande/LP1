import streamlit as st

st.title("🎓 Simple Student Management System")

# ✅ Use session_state to store data persistently
if "students" not in st.session_state:
    st.session_state.students = {}

students = st.session_state.students

# ---------------- CREATE ----------------
st.header("➕ Add New Student")

name = st.text_input("Enter Student Name:")
roll = st.text_input("Enter Roll Number:")
physics = st.number_input("Marks in Physics:", min_value=0, max_value=100, step=1)
chemistry = st.number_input("Marks in Chemistry:", min_value=0, max_value=100, step=1)
maths = st.number_input("Marks in Maths:", min_value=0, max_value=100, step=1)

if st.button("Add Student"):
    if roll.strip() == "" or name.strip() == "":
        st.warning("⚠️ Please enter both name and roll number.")
    elif roll in students:
        st.warning("⚠️ Student with this roll number already exists.")
    else:
        students[roll] = {
            "Name": name,
            "Physics": physics,
            "Chemistry": chemistry,
            "Maths": maths
        }
        st.success(f"✅ Student {name} added successfully!")
        st.balloons()

# ---------------- READ ----------------
st.header("📋 View All Students")
if students:
    for roll, info in students.items():
        st.write(f"**Roll:** {roll}")
        st.json(info)
else:
    st.info("ℹ️ No students added yet.")

# ---------------- UPDATE ----------------
st.header("✏️ Update Student")

update_roll = st.text_input("Enter Roll Number to Update:")
new_marks = st.number_input("Enter New Maths Marks:", min_value=0, max_value=100, step=1)

if st.button("Update Marks"):
    if update_roll in students:
        students[update_roll]["Maths"] = new_marks
        st.success(f"✅ Updated Maths marks for Roll {update_roll}")
    else:
        st.error("❌ Roll number not found.")

# ---------------- DELETE ----------------
st.header("🗑️ Delete Student")

del_roll = st.text_input("Enter Roll Number to Delete:")

if st.button("Delete Student"):
    if del_roll in students:
        del students[del_roll]
        st.success(f"✅ Student with Roll {del_roll} deleted.")
    else:
        st.error("❌ Roll number not found.")