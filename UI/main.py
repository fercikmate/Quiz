import serial
import tkinter as tk
from tkinter import colorchooser, messagebox
import time
#TODO send reset signal to mcu
#TODO reset protocol confirmation from mcu
#TODO add button to setup to reconfigure teams and com port
#TODO reconnect serial if disconnected
#TODO add sound effects on buzz/reset
#TODO add keyboard shortcuts for stop/resume

# Global configuration variables
com = 'COM4'
number_of_teams = 5
team_colors = ["#FF6B6B", "#D0EA05", "#45B7D1", "#0BF788", "#EC06DC"]
team_names = ["Team 1", "Team 2", "Team 3", "Team 4", "Team 5"]
ser = None # Serial port object

def show_setup_window():
    """Show setup window to configure teams and COM port"""
    global com, number_of_teams, team_colors, team_names, ser
    
    setup_window = tk.Tk()
    setup_window.title("Quiz Setup")
    setup_window.geometry("600x700")
    
    # Title
    title = tk.Label(setup_window, text="Quiz Setup", font=("Arial", 24, "bold"))
    title.pack(pady=20)
    
    # COM Port section
    com_frame = tk.Frame(setup_window)
    com_frame.pack(pady=10, padx=20, fill="x")
    tk.Label(com_frame, text="COM Port:", font=("Arial", 12)).pack(side="left", padx=5)
    com_entry = tk.Entry(com_frame, font=("Arial", 12), width=10)
    com_entry.insert(0, com)
    com_entry.pack(side="left", padx=5)
    
    # Number of teams section
    teams_frame = tk.Frame(setup_window)
    teams_frame.pack(pady=10, padx=20, fill="x")
    tk.Label(teams_frame, text="Number of Teams (1-10):", font=("Arial", 12)).pack(side="left", padx=5)
    teams_spinbox = tk.Spinbox(teams_frame, from_=1, to=10, font=("Arial", 12), width=5)
    teams_spinbox.delete(0, "end")
    teams_spinbox.insert(0, str(number_of_teams))
    teams_spinbox.pack(side="left", padx=5)
    
    # Container for team configuration
    config_container = tk.Frame(setup_window)
    config_container.pack(pady=20, padx=20, fill="both", expand=True)
    
    # Scrollable frame for teams
    canvas = tk.Canvas(config_container, height=300)
    scrollbar = tk.Scrollbar(config_container, orient="vertical", command=canvas.yview)
    scrollable_frame = tk.Frame(canvas)
    
    scrollable_frame.bind(
        "<Configure>",
        lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
    )
    
    canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
    canvas.configure(yscrollcommand=scrollbar.set)
    
    canvas.pack(side="left", fill="both", expand=True)
    scrollbar.pack(side="right", fill="y")
    
    # Team entries storage
    team_entries = []
    
    def update_team_fields():
        """Update team configuration fields based on number of teams"""
        # Clear existing fields
        for widget in scrollable_frame.winfo_children():
            widget.destroy()
        team_entries.clear()
        
        num_teams = int(teams_spinbox.get())
        
        # Default colors for new teams
        default_colors = ["#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7", 
                         "#FFA07A", "#98D8C8", "#F7DC6F", "#BB8FCE", "#85C1E2"]
        
        for i in range(num_teams):
            team_frame = tk.Frame(scrollable_frame, relief="groove", bd=2)
            team_frame.pack(pady=5, padx=10, fill="x")
            
            # Team label
            tk.Label(team_frame, text=f"Team {i+1}:", font=("Arial", 11, "bold")).grid(row=0, column=0, padx=5, pady=5, sticky="w")
            
            # Name entry
            tk.Label(team_frame, text="Name:", font=("Arial", 10)).grid(row=0, column=1, padx=5, pady=5)
            name_entry = tk.Entry(team_frame, font=("Arial", 10), width=15)
            if i < len(team_names):
                name_entry.insert(0, team_names[i])
            else:
                name_entry.insert(0, f"Team {i+1}")
            name_entry.grid(row=0, column=2, padx=5, pady=5)
            
            # Color picker
            tk.Label(team_frame, text="Color:", font=("Arial", 10)).grid(row=0, column=3, padx=5, pady=5)
            
            color_var = tk.StringVar()
            if i < len(team_colors):
                color_var.set(team_colors[i])
            else:
                color_var.set(default_colors[i % len(default_colors)])
            
            color_display = tk.Label(team_frame, bg=color_var.get(), width=3, relief="solid", bd=1)
            color_display.grid(row=0, column=4, padx=5, pady=5)
            
            def choose_color(var=color_var, display=color_display):
                color = colorchooser.askcolor(initialcolor=var.get())[1]
                if color:
                    var.set(color)
                    display.config(bg=color)
            
            color_button = tk.Button(team_frame, text="Choose", command=choose_color, font=("Arial", 9))
            color_button.grid(row=0, column=5, padx=5, pady=5)
            
            team_entries.append({"name": name_entry, "color": color_var})
    
    # Update button for teams spinbox
    teams_spinbox.config(command=update_team_fields)
    
    # Initial population
    update_team_fields()
    
    def start_quiz():
        """Validate settings and start the main quiz window"""
        global com, number_of_teams, team_colors, team_names, ser
        
        # Get COM port
        com = com_entry.get().strip()
        if not com:
            messagebox.showerror("Error", "Please enter a COM port!")
            return
        
        # Get number of teams
        try:
            number_of_teams = int(teams_spinbox.get())
            if number_of_teams < 1 or number_of_teams > 10:
                messagebox.showerror("Error", "Number of teams must be between 1 and 10!")
                return
        except ValueError:
            messagebox.showerror("Error", "Invalid number of teams!")
            return
        
        # Get team names and colors
        team_names = []
        team_colors = []
        for i, entry in enumerate(team_entries):
            name = entry["name"].get().strip()
            if not name:
                messagebox.showerror("Error", f"Please enter a name for Team {i+1}!")
                return
            team_names.append(name)
            team_colors.append(entry["color"].get())
        
        # Try to open serial port
        try:
            ser = serial.Serial(com, 9600, timeout=1)
        except Exception as e:
            messagebox.showerror("Serial Error", f" {str(e)}")
            return
        
        # Close setup window and start main window
        setup_window.destroy()
        show_main_window()
    
    # Start button
    start_button = tk.Button(setup_window, text="Start Quiz", command=start_quiz, 
                            font=("Arial", 14, "bold"), bg="#4CAF50", fg="white", 
                            padx=20, pady=10)
    start_button.pack(pady=20)
    
    setup_window.mainloop()

def show_main_window():
    """Show the main quiz window"""
    global ser
    
    window = tk.Tk()
    window.title("QUIZZER")
    window.geometry("800x600")
    
    team_timestamps = ["--:--:---"] * number_of_teams  # Stores the time when team buzzed
    buzz_order = 0  # Track the order in which teams buzz

    # Timer variables
    start_time = time.time()
    timer_running = False
    paused_elapsed = 0.0  # Track elapsed time when paused

    # Title
    title_label = tk.Label(window, text="QUIZZER", font=("Arial", 28, "bold"))
    title_label.pack(pady=10)

    # Timer display
    timer_label = tk.Label(window, text="00:00:000", font=("Arial", 48, "bold"), fg="#333")
    timer_label.pack(pady=10)

    # Container for teams
    teams_frame = tk.Frame(window)
    teams_frame.pack(fill="both", expand=True, padx=20, pady=10)

    # Store team labels for updating
    team_labels = []
    team_order_labels = []


    # Create X team sections
    for i in range(number_of_teams):
        # Configure grid columns to expand equally
        teams_frame.columnconfigure(i, weight=1)
        teams_frame.rowconfigure(0, weight=1)
        
        # Team frame
        team_frame = tk.Frame(teams_frame, bg=team_colors[i], relief="raised", bd=3)
        team_frame.grid(row=0, column=i, sticky="nsew", padx=5, pady=5)
        
        # Header frame for team name and clear button
        header_frame = tk.Frame(team_frame, bg=team_colors[i])
        header_frame.pack(fill="x", pady=5, padx=5)
        
        # Team name (left side)
        name_label = tk.Label(header_frame, text=team_names[i], font=("Arial", 14, "bold"), bg=team_colors[i])
        name_label.pack(side="left", padx=5)
        
        # Clear button (right side)
        clear_btn = tk.Button(header_frame, text="✕", command=lambda idx=i: clear_team(idx), 
                             font=("Arial", 10, "bold"), bg="#ff4444", fg="white", 
                             width=2, height=1, relief="raised", bd=1)
        clear_btn.pack(side="right", padx=5)
        
        # Team timestamp display
        time_label = tk.Label(team_frame, text="--:--:---", font=("Arial", 20, "bold"), bg=team_colors[i])
        time_label.pack(expand=True)
        team_labels.append(time_label)
        
        # Buzz order display
        order_label = tk.Label(team_frame, text="", font=("Arial", 16, "bold"), bg=team_colors[i])
        order_label.pack(pady=5)
        team_order_labels.append(order_label)
    

    # Status label for serial data
    label = tk.Label(window, text="Waiting for input...", font=("Arial", 16))
    label.pack(pady=20)

    def format_time(elapsed):
        """Format elapsed time as MM:SS:mmm"""
        minutes = int(elapsed // 60)
        seconds = int(elapsed % 60)
        millis = int((elapsed * 1000) % 1000)
        return f"{minutes:02d}:{seconds:02d}:{millis:03d}"

    def update_timer():
        """Update the timer display"""
        nonlocal timer_running, paused_elapsed
        if timer_running:
            elapsed = time.time() - start_time
            paused_elapsed = elapsed  # Keep track of current elapsed time
            timer_label.config(text=format_time(elapsed))
        
        if window.winfo_exists():
            window.after(1, update_timer)  # Update every 10ms

    def read_serial():
        """Read serial data and stamp team times"""
        nonlocal start_time, timer_running, paused_elapsed, buzz_order
        try:
            if ser and ser.is_open and ser.in_waiting > 0:  # Check if data is available
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    try:
                        number = int(line)
                        # If number is 1-x, stamp that team's time
                        if 1 <= number <= number_of_teams:
                            team_idx = number - 1
                            # Only stamp if not already stamped
                            if team_timestamps[team_idx] == "--:--:---":
                                # Use paused time if stopped, otherwise calculate current elapsed
                                if timer_running:
                                    elapsed = time.time() - start_time
                                else:
                                    elapsed = paused_elapsed
                                team_timestamps[team_idx] = format_time(elapsed)
                                team_labels[team_idx].config(text=team_timestamps[team_idx])
                                
                                # Increment and display buzz order
                                buzz_order += 1
                                team_order_labels[team_idx].config(text=f"{buzz_order}.")
                                
                                label.config(text=f"{team_names[team_idx]} buzzed in!")
                        else:
                            label.config(text=f"Received: {number}")
                    except ValueError:
                        pass  # Ignore non-integer data
        except (serial.SerialException, OSError) as e:
            label.config(text=f"Serial error: disconnected")
        except Exception as e:
            pass  # Ignore other errors silently
        
        if window.winfo_exists():
            window.after(10, read_serial)  # Check again in 10ms

    def on_click_reset():
        """Reset timer and all team timestamps"""
        nonlocal start_time, team_timestamps, timer_running, paused_elapsed, buzz_order
        start_time = time.time()
        timer_running = False
        paused_elapsed = 0.0
        buzz_order = 0
        team_timestamps = ["--:--:---"] * number_of_teams
        for i in range(number_of_teams):
            team_labels[i].config(text="--:--:---")
            team_order_labels[i].config(text="")
        timer_label.config(text="00:00:000")
        label.config(text="Timer reset - Click Resume to start")
        button_stop.config(text="Resume", bg="#4CAF50")

    def on_click_stop():
        """Stop/Resume the timer"""
        nonlocal timer_running, start_time, paused_elapsed
        timer_running = not timer_running
        if timer_running:
            # Resuming: adjust start_time to account for paused duration
            start_time = time.time() - paused_elapsed
            button_stop.config(text="Stop", bg="#f44336")
            label.config(text="Timer resumed")
        else:
            # Stopping: paused_elapsed is already updated in update_timer
            button_stop.config(text="Resume", bg="#4CAF50")
            label.config(text="Timer stopped")

    # Cleanup handler
    def on_closing():

        """Handle window close event"""
        try:
            if ser and ser.is_open:
                ser.close()
        except Exception:
            pass  # Ignore cleanup errors
        window.destroy()
    
    def clear_team(team_idx):
        """Clear a specific team's timestamp and order"""
        nonlocal team_timestamps, buzz_order
        if team_timestamps[team_idx] != "--:--:---":
            team_timestamps[team_idx] = "--:--:---"
            team_labels[team_idx].config(text="--:--:---")
            
            # Get the order that's being cleared
            old_order_text = team_order_labels[team_idx].cget("text")
            if old_order_text:
                try:
                    old_order = int(old_order_text.replace(".", ""))
                    
                    # Decrement orders for all teams that buzzed after this one
                    for j in range(number_of_teams):
                        current_order_text = team_order_labels[j].cget("text")
                        if current_order_text:
                            current_order = int(current_order_text.replace(".", ""))
                            if current_order > old_order:
                                team_order_labels[j].config(text=f"{current_order - 1}.")
                    
                    # Decrement the buzz_order counter (prevent negative)
                    buzz_order = max(0, buzz_order - 1)
                except ValueError:
                    pass  # Ignore if order text is malformed
            
            team_order_labels[team_idx].config(text="")
            label.config(text=f"{team_names[team_idx]} cleared")
    
    # Button frame for Reset and Stop
    button_frame = tk.Frame(window)
    button_frame.pack(pady=10)
    
    button_rst = tk.Button(button_frame, text="Reset", command=on_click_reset, font=("Arial", 14), padx=20)
    button_rst.pack(side="left", padx=5)
    
    button_stop = tk.Button(button_frame, text="Resume", command=on_click_stop, font=("Arial", 14), bg="#4CAF50", fg="white", padx=20)
    button_stop.pack(side="left", padx=5)

    # Start the timer and serial reading
    update_timer()
    read_serial()


    
    window.protocol("WM_DELETE_WINDOW", on_closing)
    
    # Run the app
    window.mainloop()

# Start with the setup window
if __name__ == "__main__":
    show_setup_window()