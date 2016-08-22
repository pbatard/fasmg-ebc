;
; EBC (EFI Byte Code) assembler definition for fasmg
; Copyright © 2016 Pete Batard <pete@akeo.ie>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

element R

repeat 8 i:0
  element R#i? : R + i
end repeat

macro ebc.parse_register reg_num, Rn
  local value
  value = +Rn
  reg_num = value metadata 1 - R
end macro

macro ebc.move_immediate mov_width, imm_width, dst, imm
  local reg_num, indirect
  db 0x37 + imm_width shl 6
  match [reg], dst
    ebc.parse_register reg_num, reg
    indirect = 1
  else
    ebc.parse_register reg_num, dst
    indirect = 0
  end match
  db mov_width shl 4 + indirect shl 3 + reg_num
  match =3?, imm_width
    dq imm
  else match =2?, imm_width
    dd imm
  else match =1?, imm_width
    dw imm
  end match
end macro

macro MOVIbw? dst*, imm*
  ebc.move_immediate 0, 1, dst, imm
end macro

macro MOVIbd? dst*, imm*
  ebc.move_immediate 0, 2, dst, imm
end macro

macro MOVIbq? dst*, imm*
  ebc.move_immediate 0, 3, dst, imm
end macro

macro MOVIww? dst*, imm*
  ebc.move_immediate 1, 1, dst, imm
end macro

macro MOVIwd? dst*, imm*
  ebc.move_immediate 1, 2, dst, imm
end macro

macro MOVIwq? dst*, imm*
  ebc.move_immediate 1, 3, dst, imm
end macro

macro MOVIdw? dst*, imm*
  ebc.move_immediate 2, 1, dst, imm
end macro

macro MOVIdd? dst*, imm*
  ebc.move_immediate 2, 2, dst, imm
end macro

macro MOVIdq? dst*, imm*
  ebc.move_immediate 2, 3, dst, imm
end macro

macro MOVIqw? dst*, imm*
  ebc.move_immediate 3, 1, dst, imm
end macro

macro MOVIqd? dst*, imm*
  ebc.move_immediate 3, 2, dst, imm
end macro

macro MOVIqq? dst*, imm*
  ebc.move_immediate 3, 3, dst, imm
end macro

macro RET?
  dw 0x0004
end macro
