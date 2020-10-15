onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -format Analog-Step -height 74 -max 14.999999999999998 -radix unsigned /vga_ram_tb/uut/charrowcounter/glyph_row
add wave -noupdate -format Analog-Step -height 74 -max 7.0 -radix unsigned /vga_ram_tb/uut/charrowcounter/char_row
add wave -noupdate -format Analog-Step -height 74 -max 30.999999999999996 -radix unsigned /vga_ram_tb/uut/charrowcounter/cur_row
add wave -noupdate -format Analog-Step -height 74 -max 4720.0 -radix unsigned /vga_ram_tb/uut/charrowcounter/cur_char
add wave -noupdate -format Analog-Step -height 74 -max 799.0 -radix unsigned /vga_ram_tb/uut/vgacontrol/h_addr
add wave -noupdate -format Analog-Step -height 74 -max 524.0 -radix unsigned /vga_ram_tb/uut/vgacontrol/v_addr
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {2240040000 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 328
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {1768248334 ps} {4400145698 ps}
