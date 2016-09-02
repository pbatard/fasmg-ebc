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
  element @R#i? : R + i + 0x80
end repeat

macro ebc.parse_register reg_num, indirect, Rn
  local value
  value = +Rn
  reg_num = value metadata 1 - R
  if (reg_num >= 0x80)
    reg_num = reg_num - 0x80
    indirect = 1
  else
    indirect = 0
  end if
end macro

macro di width, val
  if (width = 0)
    db val
  else if (width = 1)
    dw val
  else if (width = 2)
    dl val
  else if (width = 3)
    dq val
  end if
end macro

macro ebc.insert_index index_size, nat, cst
  local abs_cst, abs_nat, sign, width, nbits
  sign = 0
  nbits = index_size / 8
  if ((cst < 0) & (nat > 0)) | ((cst > 0) & (nat < 0))
     err 'constant and natural values must have the same sign'
  end if
  if (cst < 0)
    sign = 1
    cst = -cst
    nat = -nat
  end if
  width = 8
  if (nat = 0)
    width = 0
  else
    repeat 7
      if (nat < (1 shl (% * nbits)))
        width = %
        break
      end if
    end repeat
  end if
  if (width > 7) | ((width = 7) & (index_size = 16))
    err 'natural value too large for index size'
  end if
  if (cst >= 1 shl (index_size - 4 - (width*2)))
    err 'constant and natural values too large for index size'
  end if
  di (index_size/16), sign shl (index_size-1) + width shl (index_size-4) + cst shl (width*nbits) + nat
end macro

macro ebc.move_immediate mov_width, imm_width, op1_full, op2
  local op1, op1_reg_num, op1_indirect, op1_index, op1_nat, op1_cst
  db 0x37 + imm_width shl 6
  op1_nat = 0
  op1_cst = 0
  match op1_short (n:c), op1_full
    op1_index = 1
    define op1 op1_short
    op1_nat = n
    op1_cst = c
  else
    op1_index = 0
    define op1 op1_full
  end match
  ebc.parse_register op1_reg_num, op1_indirect, op1
  db op1_index shl 6 + mov_width shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_nat <> 0) | (op1_cst <> 0)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  di imm_width, op2
end macro

macro ebc.move_general opcode, op1, op2
  local op1_reg_num, op2_reg_num, op1_indirect, op2_indirect
  ; TODO: add index for op1 & op2
  db opcode
  local int_val
  ebc.parse_register op1_reg_num, op1_indirect, op1
  ebc.parse_register op2_reg_num, op2_indirect, op2
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num
end macro

macro MOVIbw? op1*, op2*
  ebc.move_immediate 0, 1, op1, op2
end macro

macro MOVIbd? op1*, op2*
  ebc.move_immediate 0, 2, op1, op2
end macro

macro MOVIbq? op1*, op2*
  ebc.move_immediate 0, 3, op1, op2
end macro

macro MOVIww? op1*, op2*
  ebc.move_immediate 1, 1, op1, op2
end macro

macro MOVIwd? op1*, op2*
  ebc.move_immediate 1, 2, op1, op2
end macro

macro MOVIwq? op1*, op2*
  ebc.move_immediate 1, 3, op1, op2
end macro

macro MOVIdw? op1*, op2*
  ebc.move_immediate 2, 1, op1, op2
end macro

macro MOVIdd? op1*, op2*
  ebc.move_immediate 2, 2, op1, op2
end macro

macro MOVIdq? op1*, op2*
  ebc.move_immediate 2, 3, op1, op2
end macro

macro MOVIqw? op1*, op2*
  ebc.move_immediate 3, 1, op1, op2
end macro

macro MOVIqd? op1*, op2*
  ebc.move_immediate 3, 2, op1, op2
end macro

macro MOVIqq? op1*, op2*
  ebc.move_immediate 3, 3, op1, op2
end macro

macro MOVqq? op1*, op2*
  ebc.move_general 0x28, op1, op2
end macro

macro RET?
  dw 0x0004
end macro
