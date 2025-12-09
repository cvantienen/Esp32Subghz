# Arduino Loop() Function - Study Notes

## 🎯 **Big Picture: What This Program Does**
This is a **menu-driven signal transmitter** with 4 screens that you navigate using UP, DOWN, and SELECT buttons:
- **Screen 0**: Browse categories (like "Remotes", "Garage Doors")
- **Screen 1**: Browse signals within a category (like "Remote A", "Remote B")
- **Screen 2**: View signal details before sending
- **Screen 3**: Actually transmit the signal

---

## 🔄 **Program Flow Diagram**
```
Screen 0 (Categories)
    ↓ [SELECT]
Screen 1 (Signals in category)
    ↓ [SELECT]
Screen 2 (Signal details)
    ↓ [SELECT]
Screen 3 (Transmitting...)
    ↓ [AUTO after 0.8s]
Back to Screen 1
```

---

## 🎮 **Button Mechanics (Debouncing Pattern)**

### How Button Detection Works:
```cpp
if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0))
```
This checks TWO things:
1. **Is button physically pressed?** (`digitalRead() == LOW`)
2. **Haven't we already registered this press?** (`button_up_clicked == 0`)

### Why the `button_X_clicked` flags?
Without these flags, loop() runs ~10,000 times per second. One button press would register hundreds of times! The flags prevent this:

```
Press button → clicked = 1 (ignore further presses)
       ↓
Release button → clicked = 0 (ready for next press)
```

**Key Pattern:**
```cpp
if (button pressed AND not already clicked) {
    // Do the action
    button_clicked = 1;  // Prevent repeats
}
if (button released) {
    button_clicked = 0;  // Reset for next press
}
```

---

## 📋 **Screen-by-Screen Breakdown**

### **SCREEN 0: Category Menu**
```
Variables involved:
- category_selected (current selection)
- category_sel_previous (item above)
- category_sel_next (item below)
```

**What happens:**
- UP button: `category_selected--` (with wraparound)
- DOWN button: `category_selected++` (with wraparound)
- SELECT button: Go to Screen 1

**Wraparound Logic:**
```cpp
if (category_selected < 0) {
    category_selected = NUM_CATEGORIES - 1;  // Jump to end
}
```
Example: If there are 5 categories (0-4), pressing UP on category 0 wraps to category 4.

---

### **SCREEN 1: Signal Submenu**
```
Variables involved:
- signal_selected (current signal)
- signal_sel_previous (signal above)
- signal_sel_next (signal below)
- num_signals (how many signals in this category)
```

**What happens:**
- UP/DOWN: Navigate through signals in current category
- SELECT: Go to Screen 2 (details)

**Important:** When you first enter Screen 1, `signal_selected = 0` (resets to first signal)

---

### **SCREEN 2: Signal Details**
This is a "review" screen showing:
- Signal name
- Category name
- Frequency (e.g., "433.92 MHz")
- Number of samples
- Instructions: "SELECT to transmit"

**What happens:**
- UP/DOWN: Do nothing (no navigation on this screen)
- SELECT: Go to Screen 3 (transmit)

---

### **SCREEN 3: Transmit**
This screen **does two things**:
1. **Shows UI**: "SENDING..." then "SENT!"
2. **Transmits signal**: Calls `radio.transmit()`

**Critical difference:** This screen has **business logic** (actual radio transmission), not just UI.

**Flow:**
```
1. Show "SENDING..." → sendBuffer()
2. Transmit signal → radio.transmit()
3. Show "SENT!" → sendBuffer()
4. Wait 800ms → delay(800)
5. Auto-return to Screen 1
```

---

## 🔢 **The "Prev/Next" Calculation Block**

This happens **every loop**, regardless of screen:

```cpp
// Category prev/next
category_sel_previous = category_selected - 1;
if (category_sel_previous < 0) {
    category_sel_previous = NUM_CATEGORIES - 1;
}
// ... similar for category_sel_next

// Signal prev/next  
// ... similar logic
```

**Why calculate these every loop?**
The UI shows 3 items at once (previous, current, next), so we need these values ready for drawing.

**Visual Example:**
```
  [Remotes]     ← category_sel_previous
→ [Garage]      ← category_selected (bold/highlighted)
  [Car Keys]    ← category_sel_next
```

---

## 🎨 **The Drawing Section**

```cpp
display.clear();  // Wipe screen

if (current_screen == 0) {
    display.drawCategoryMenu(...);
}
else if (current_screen == 1) {
    // Manual u8g2 drawing code
}
// ... etc

u8g2.sendBuffer();  // Actually show everything
delay(10);          // Small pause
```

**Important:** There are TWO display systems here:
1. `display.drawCategoryMenu()` - newer abstracted method
2. Direct `u8g2.drawStr()` calls - older manual drawing

---

## 🐛 **Known Issues in Code**

### **Issue 1: Conflicting Screen 1 Code**
There are TWO different implementations for Screen 1:
- Lines ~149-160: Shows signal details in a list
- Lines ~168-195: Shows prev/current/next signals

The second one overwrites the first! Only the last one actually displays.

### **Issue 2: Undefined `signals` Array**
```cpp
u8g2.drawStr(8, 26, signals[signal_sel_previous].name);  // ❌ WRONG
```
Should be:
```cpp
u8g2.drawStr(8, 26, CATEGORIES[category_selected].signals[signal_sel_previous].name);
```

---

## 💡 **Key Concepts to Remember**

### **1. State Machine Pattern**
The `current_screen` variable determines what the program does:
```
current_screen = 0 → Category navigation active
current_screen = 1 → Signal navigation active
current_screen = 2 → Display-only (no navigation)
current_screen = 3 → Transmission + auto-advance
```

### **2. Circular Arrays (Wraparound)**
All menus wrap around:
```
[0] → [1] → [2] → [3] → [0] → ...
 ↑__________________________|
```

### **3. Reset on Entry**
When entering Screen 1, `signal_selected = 0` ensures you always start at the first signal of a category.

### **4. Loop Runs Continuously**
This function executes ~10,000 times per second:
- Checks buttons every cycle
- Redraws screen every cycle (though display only updates if changed)
- `delay(10)` slows it slightly

---

## 🔍 **Tracing a User Action**

**Example: User wants to transmit "Garage Remote A"**

1. **Start**: Screen 0, category_selected = 0 ("Remotes")
2. **Press DOWN**: category_selected = 1 ("Garage Doors")
3. **Press SELECT**: current_screen = 1, signal_selected = 0
4. **Screen 1 displays**: "Garage Remote A" (first signal)
5. **Press SELECT**: current_screen = 2
6. **Screen 2 displays**: Details of "Garage Remote A"
7. **Press SELECT**: current_screen = 3
8. **Screen 3**:
   - Shows "SENDING..."
   - Calls `radio.transmit()`
   - Shows "SENT!"
   - Waits 800ms
   - Returns to Screen 1

---

## 📚 **Study Tips**

1. **Follow one button press** through the entire code
2. **Draw the screen transitions** on paper
3. **Identify the pattern**: Check button → Update variable → Draw screen
4. **Notice the repetition**: Screen 0 and Screen 1 have nearly identical navigation logic
5. **Understand the flags**: Every `button_X_clicked` flag prevents bounce`