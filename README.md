# **SFML Falling Objects Game** 🎮

A fun and dynamic game developed using **C++** and **SFML**, where players must click on falling objects to gain points while avoiding missing them. The game includes health tracking, high-score saving, and smooth animations.

---

## 🚀 **Features**
- 🎯 **Challenging Gameplay**: Objects fall at increasing speeds, providing an engaging experience.  
- 🖱️ **Mouse Click Interaction**: Click on objects to gain points.  
- ❤️ **Health System**: Lose health when objects reach the bottom.  
- 📊 **High Score Tracking**: Saves your highest score to a file.  
- 🎮 **Smooth SFML Graphics**: Uses SFML for rendering and event handling.  
- 🕹️ **Simple Controls**: Intuitive and easy-to-learn mechanics.

---

## 🛠 **Tech Stack**
- **Programming Language**: C++  
- **Graphics Library**: SFML  

---

## 🌐 **Run On GitHub Pages**
This repository now includes a browser version in `web/` and an automatic GitHub Pages deployment workflow.
The original C++ SFML game remains the desktop/native build path.

### **1. Enable Pages in repo settings**
1. Open **Settings** → **Pages** in your GitHub repository.
2. Under **Build and deployment**, set **Source** to **GitHub Actions**.

### **2. Push to `main`**
When you push to `main`, GitHub will run:
- `.github/workflows/pages.yml` to deploy `web/`
- `.github/workflows/cmake-single-platform.yml` for native C++ CI

### **3. Open your site**
- Project pages URL: `https://<username>.github.io/<repo-name>/`
- User/organization pages URL: `https://<username>.github.io/`

### **4. Custom domain (optional)**
1. Add your domain in **Settings** → **Pages** → **Custom domain**.
2. Create/update DNS records at your domain provider.
3. Add a `CNAME` file inside `web/` containing only your domain (example: `game.example.com`).

---

## 📥 **Installation & Setup**
### **1. Install SFML**
Make sure you have SFML installed on your system. If you don’t, you can install it as follows:
```sh
sudo apt-get install libsfml-dev # Ubuntu
brew install sfml # macOS
choco install sfml # Windows (via Chocolatey)
```

### **2. Clone the Repository**
```sh
git clone https://github.com/yourusername/sfml-falling-game.git
cd sfml-falling-game
```

### **3. Compile and Run**
Compile the project using `g++` and run the game:
```sh
g++ -o game main.cpp Game.cpp -lsfml-graphics -lsfml-window -lsfml-system
./game
```
If using **CMake**, configure and build:
```sh
mkdir build && cd build
cmake ..
make
./game
```

---

## 🎮 **Gameplay Instructions**
- **Objective**: Click on falling objects to score points before they reach the ground.  
- **Controls**:
  - 🖱️ **Left Mouse Click**: Click objects to gain points.  
  - ⬅️➡️ **Arrow Keys**: Move the basket (if enabled in the game).  
- **Scoring System**:
  - ✅ Successfully clicking objects increases your score.  
  - ❌ Missing objects reduces your health.  
  - ❤️ If health reaches 0, the game ends.

---

## 📂 **Project Structure**
```plaintext
.
├── main.cpp       # Game loop and main execution file
├── Game.cpp       # Game logic and rendering functions
├── Game.h         # Header file for Game class
├── database/      # Directory to store high-score data
├── Fonts/         # Directory for font files
├── README.md      # Project documentation
```

---

## 🤝 **Contributing**
Want to enhance the game? Follow these steps:
1. Fork the repository.  
2. Create a feature branch:  
   ```sh
   git checkout -b feature-name
   ```  
3. Commit and push changes:  
   ```sh
   git commit -m "Add new feature"
   git push origin feature-name
   ```  
4. Submit a pull request.  

---

## 📜 **License**
This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## 📧 **Contact**
**Author**: Tamim Orif  
**GitHub**: [tamimorif](https://github.com/tamimorif/Falling-Fury)  

For suggestions or collaborations, feel free to reach out! 🚀
