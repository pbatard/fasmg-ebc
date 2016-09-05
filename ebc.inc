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

;
; Supporting macros
;

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
    dd val
  else
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
  ; NB: we abuse the fact that 'di' inserts a 'dq' for size 3 or 4
  di (index_size/16), sign shl (index_size-1) + width shl (index_size-4) + cst shl (width*nbits) + nat
end macro

macro ebc.move_imm_ind_rel mov_width, op2_width, ops&
  local op1, op2, op1_reg_num, op1_indirect, op1_index, op2_index
  local op1_nat, op2_nat, op1_cst, op2_cst, opcode, ins_width
  if (mov_width >= 0)
    opcode = 0x37
    ins_width = mov_width
  else if (mov_width = -1)
    opcode = 0x38
    ins_width = 0
  else if (mov_width = -2)
    opcode = 0x39
    ins_width = 0
  end if
  db op2_width shl 6 + opcode
  op1_nat = 0
  op1_cst = 0

  match _op1 (n1 =, c1) =, (n2 =, c2), ops
    if (mov_width <> -1)
      err 'syntax error'
    end if
    op1_index = 1
    define op1 _op1
    define op2 _op2
    op1_nat = n1
    op1_cst = c1
    op2_nat = n2
    op2_cst = c2
  else match _op1 (n1 =, c1) =, _op2, ops
    op1_index = 1
    define op1 _op1
    define op2 _op2
    op1_nat = n1
    op1_cst = c1
  else match _op1 =, ( n2 =, c2), ops
    if (mov_width <> -1)
      err 'syntax error'
    end if
    op1_index = 0
    define op1 _op1
    op2_nat = n2
    op2_cst = c2
  else match _op1 =, _op2, ops
    op1_index = 0
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'index specified for direct register operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if

  db op1_index shl 6 + ins_width shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  if (mov_width <> -1)
    di op2_width, op2
  else
    ebc.insert_index (1 shl op2_width)*8, op2_nat, op2_cst
  end if
end macro

macro ebc.move_general opcode, ops&
  local op1, op2, op1_reg_num, op2_reg_num, op1_indirect, op2_indirect, op2_imm
  local op1_index, op2_index, op1_nat, op2_nat, op1_cst, op2_cst, index_size
  op1_index = 0
  op2_index = 0
  op1_nat = 0
  op2_nat = 0
  op1_cst = 0
  op2_cst = 0
  op2_imm = 0
  match _op1 (n1 =, c1) =, _op2 (n2 =, c2), ops
    op1_index = 1
    op1_nat = n1
    op1_cst = c1
    op2_index = 1
    op2_nat = n2
    op2_cst = c2
    define op1 _op1
    define op2 _op2
  else match _op1 (n1 =, c1) =, _op2 (_imm), ops
    if (opcode <> 0x25) & (opcode <> 0x26)
      err 'syntax error'
    end if
    op1_index = 1
    op1_nat = n1
    op1_cst = c1
    op2_index = 3
    define op1 _op1
    define op2 _op2
    op2_imm = _imm
  else match _op1 (n1 =, c1) =, _op2, ops
    op1_index = 1
    op1_nat = n1
    op1_cst = c1
    define op1 _op1
    define op2 _op2
  else match _op1 =, _op2 (n2 =, c2), ops
    op2_index = 1
    op2_nat = n2
    op2_cst = c2
    define op1 _op1
    define op2 _op2
  else match _op1 =, _op2 (_imm), ops
    if (opcode <> 0x25) & (opcode <> 0x26)
      err 'syntax error'
    end if
    op2_index = 3
    define op1 _op1
    define op2 _op2
    op2_imm = _imm
  else match _op1 =, _op2, ops
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  ebc.parse_register op2_reg_num, op2_indirect, op2
  if ((op1_index = 1) & (op1_indirect <> 1)) | ((op2_index = 1) & (op2_indirect <> 1))
    err 'index specified for direct register operation'
  else if (op2_index = 3) & (op2_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if
  if (opcode > 0x100) & ((op1_index = 1) | (op2_index = 1))
    err 'index provided but no index size specified'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op2_index = 1) & (op2_nat = 0) & (op2_cst = 0)
    op2_index = 0
  end if

  db op1_index shl 7 + (op2_index and 1) shl 6 + (opcode and 0xff)
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num

  index_size = 16
  if ((opcode >= 0x21) & (opcode <= 0x24)) | (opcode = 0x26) | (opcode = 0x33)
    index_size = 32
  else if (opcode = 0x28)
    index_size = 64
  end if

  if (op1_index = 1)
    ebc.insert_index index_size, op1_nat, op1_cst
  end if
  if (op2_index = 1)
    ebc.insert_index index_size, op2_nat, op2_cst
  else if (op2_index = 3)
    di (opcode - 0x24), op2_imm
  end if
end macro

macro ebc.math_general opcode, ops&
  local op1, op2, op1_reg_num, op2_reg_num, op1_indirect, op2_indirect
  local op2_index, op2_nat, op2_cst, op2_imm
  op2_nat = 0
  op2_cst = 0
  op2_imm = 0
  is_64 = 0
  match _op1 =, _op2 (n2 =, c2), ops
    op2_index = 1
    define op1 _op1
    define op2 _op2
    op2_nat = n2
    op2_cst = c2
  else match _op1 =, _op2 (_imm), ops
    op2_index = 3
    define op1 _op1
    define op2 _op2
    op2_imm = _imm
  else match _op1 =, _op2, ops
    op2_index = 0
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  ebc.parse_register op2_reg_num, op2_indirect, op2
  if (op2_index = 1) & (op2_indirect <> 1)
    err 'index specified for direct register operation'
  end if
  if (op2_nat = 0) & (op2_cst = 0) & (op2_imm = 0)
    op2_index = 0
  end if

  db (op2_index and 1) shl 7 + opcode
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op2_index = 1)
    ebc.insert_index 16, op2_nat, op2_cst
  else if (op2_index = 3)
    dw op2_imm
  end if
end macro

;
; Instruction list
;

macro ADD32? ops&
  ebc.math_general 0x0C, ops
end macro

macro ADD64? ops&
  ebc.math_general 0x4C, ops
end macro

macro AND32? ops&
  ebc.math_general 0x14, ops
end macro

macro AND64? ops&
  ebc.math_general 0x54, ops
end macro

macro ASHR32? ops&
  ebc.math_general 0x19, ops
end macro

macro ASHR64? ops&
  ebc.math_general 0x59, ops
end macro

macro DIV32? ops&
  ebc.math_general 0x10, ops
end macro

macro DIV64? ops&
  ebc.math_general 0x50, ops
end macro

macro DIVU32? ops&
  ebc.math_general 0x11, ops
end macro

macro DIVU64? ops&
  ebc.math_general 0x51, ops
end macro

macro EXTNDB32? ops&
  ebc.math_general 0x1A, ops
end macro

macro EXTNDB64? ops&
  ebc.math_general 0x5A, ops
end macro

macro EXTNDD32? ops&
  ebc.math_general 0x1C, ops
end macro

macro EXTNDD64? ops&
  ebc.math_general 0x5C, ops
end macro

macro EXTNDW32? ops&
  ebc.math_general 0x1B, ops
end macro

macro EXTNDW64? ops&
  ebc.math_general 0x5B, ops
end macro

macro MOD32? ops&
  ebc.math_general 0x12, ops
end macro

macro MOD64? ops&
  ebc.math_general 0x52, ops
end macro

macro MODU32? ops&
  ebc.math_general 0x13, ops
end macro

macro MODU64? ops&
  ebc.math_general 0x53, ops
end macro

macro MOVbw? ops&
  ebc.move_general 0x1D, ops
end macro

macro MOVb? ops&
  ebc.move_general 0x11D, ops
end macro

macro MOVww? ops&
  ebc.move_general 0x1E, ops
end macro

macro MOVw? ops&
  ebc.move_general 0x11E, ops
end macro

macro MOVdw? ops&
  ebc.move_general 0x1F, ops
end macro

macro MOVd? ops&
  ebc.move_general 0x11F, ops
end macro

macro MOVqw? ops&
  ebc.move_general 0x20, ops
end macro

macro MOVq? ops&
  ebc.move_general 0x120, ops
end macro

macro MOVbd? ops&
  ebc.move_general 0x21, ops
end macro

macro MOVwd? ops&
  ebc.move_general 0x22, ops
end macro

macro MOVdd? ops&
  ebc.move_general 0x23, ops
end macro

macro MOVqd? ops&
  ebc.move_general 0x23, ops
end macro

macro MOVqq? ops&
  ebc.move_general 0x28, ops
end macro

macro MOVIbw? ops&
  ebc.move_imm_ind_rel 0, 1, ops
end macro

macro MOVIbd? ops&
  ebc.move_imm_ind_rel 0, 2, ops
end macro

macro MOVIbq? ops&
  ebc.move_imm_ind_rel 0, 3, ops
end macro

macro MOVIww? ops&
  ebc.move_imm_ind_rel 1, 1, ops
end macro

macro MOVIwd? ops&
  ebc.move_imm_ind_rel 1, 2, ops
end macro

macro MOVIwq? ops&
  ebc.move_imm_ind_rel 1, 3, ops
end macro

macro MOVIdw? ops&
  ebc.move_imm_ind_rel 2, 1, ops
end macro

macro MOVIdd? ops&
  ebc.move_imm_ind_rel 2, 2, ops
end macro

macro MOVIdq? ops&
  ebc.move_imm_ind_rel 2, 3, ops
end macro

macro MOVIqw? ops&
  ebc.move_imm_ind_rel 3, 1, ops
end macro

macro MOVIqd? ops&
  ebc.move_imm_ind_rel 3, 2, ops
end macro

macro MOVIqq? ops&
  ebc.move_imm_ind_rel 3, 3, ops
end macro

macro MOVInw? ops&
  ebc.move_imm_ind_rel -1, 1, ops
end macro

macro MOVInd? ops&
  ebc.move_imm_ind_rel -1, 2, ops
end macro

macro MOVInq? ops&
  ebc.move_imm_ind_rel -1, 3, ops
end macro

macro MOVnw? ops&
  ebc.move_general 0x32, ops
end macro

macro MOVnd? ops&
  ebc.move_general 0x33, ops
end macro

macro MOVRELw? ops&
  ebc.move_imm_ind_rel -2, 1, ops
end macro

macro MOVRELd? ops&
  ebc.move_imm_ind_rel -2, 2, ops
end macro

macro MOVRELq? ops&
  ebc.move_imm_ind_rel -2, 3, ops
end macro

macro MOVsnw? ops&
  ebc.move_general 0x25, ops
end macro

macro MOVsnd? ops&
  ebc.move_general 0x26, ops
end macro

macro MUL32? ops&
  ebc.math_general 0x0E, ops
end macro

macro MUL64? ops&
  ebc.math_general 0x4E, ops
end macro

macro MULU32? ops&
  ebc.math_general 0x0F, ops
end macro

macro MULU64? ops&
  ebc.math_general 0x4F, ops
end macro

macro NEG32? ops&
  ebc.math_general 0x0B, ops
end macro

macro NEG64? ops&
  ebc.math_general 0x4B, ops
end macro

macro NOT32? ops&
  ebc.math_general 0x0A, ops
end macro

macro NOT64? ops&
  ebc.math_general 0x4A, ops
end macro

macro OR32? ops&
  ebc.math_general 0x15, ops
end macro

macro OR64? ops&
  ebc.math_general 0x55, ops
end macro

macro RET?
  dw 0x0004
end macro

macro SHL32? ops&
  ebc.math_general 0x17, ops
end macro

macro SHL64? ops&
  ebc.math_general 0x57, ops
end macro

macro SHR32? ops&
  ebc.math_general 0x18, ops
end macro

macro SHR64? ops&
  ebc.math_general 0x58, ops
end macro

macro SUB32? ops&
  ebc.math_general 0x0D, ops
end macro

macro SUB64? ops&
  ebc.math_general 0x4D, ops
end macro

macro XOR32? ops&
  ebc.math_general 0x16, ops
end macro

macro XOR64? ops&
  ebc.math_general 0x56, ops
end macro
