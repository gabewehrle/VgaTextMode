onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /vga_ram_tb/clk
add wave -noupdate /vga_ram_tb/vgaclk
add wave -noupdate /vga_ram_tb/uut/charcounter/Q
add wave -noupdate /vga_ram_tb/uut/charrowcounter/char_row
add wave -noupdate /vga_ram_tb/uut/char_frame_buffer/rdaddress
add wave -noupdate /vga_ram_tb/uut/glyph_rom_inst/address
add wave -noupdate /vga_ram_tb/uut/char_code
add wave -noupdate /vga_ram_tb/uut/char_info
add wave -noupdate /vga_ram_tb/uut/glyph_row_addr
add wave -noupdate /vga_ram_tb/uut/char_row
add wave -noupdate /vga_ram_tb/uut/valid
add wave -noupdate /vga_ram_tb/uut/VGA_R
add wave -noupdate /vga_ram_tb/uut/charrowcounter/glyph_row
add wave -noupdate /vga_ram_tb/uut/glyph_row
add wave -noupdate /vga_ram_tb/uut/pixel
add wave -noupdate /vga_ram_tb/uut/char_frame_buffer/q
add wave -noupdate /vga_ram_tb/uut/char_frame_buffer/rdaddress
add wave -noupdate /vga_ram_tb/uut/char_frame_buffer/clock
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ns} 0}
quietly wave cursor active 0
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
WaveRestoreZoom {0 ns} {2392 ns}
