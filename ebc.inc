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
  element R#i?  : R + i
  element @R#i? : R + i + 0x80
end repeat

element FLAGS?  : R + 0x800
element IP?     : R + 0x801

;
; Supporting macros
;

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

macro abs abs_val, val
  if (val < 0)
    abs_val = -cst
  else
    abs_val = val
  end if
end macro

macro is_reloc ret, value
  local v
  v = value
  if ((~ v relativeto 0) | (v relativeto PE.RELOCATION)) & (~ v metadata 1 relativeto R)
    ret = 1
  else
    ret = 0
  end if
end macro

macro is_immediate ret, ops&
  match a =, b =, c, ops
    ret = 0
  else match a =, b, ops
    ret = 0
  else match a ( b ), ops
    ret = 0
  else match v, ops
     if ((~ v relativeto 0) | (v relativeto PE.RELOCATION)) & (~ v metadata 1 relativeto R)
       ret = 1
     else
       ret = 0
     end if
  else
    ret = 0
  end match
end macro

macro ebc.parse_register reg_num, indirect_or_vm, reg
  local value
  value = +reg
  reg_num = value metadata 1 - R
  if (reg_num >= 0x800)
    indirect_or_vm = 2
  else if (reg_num >= 0x80)
    indirect_or_vm = 1
  else
    indirect_or_vm = 0
  end if
  reg_num = (reg_num and 0x07)
end macro

macro ebc.get_index_size index_size, nat, cst
  local abs_cst, abs_nat, used_bits, max_n
  abs abs_cst, cst
  abs abs_nat, nat
  used_bits = 4 ; 4 bits for sign + natural width
  while (abs_cst > 0)
    used_bits = used_bits + 1
    abs_cst = abs_cst shr 1
  end while
  index_size = 16
  repeat 3 i:1
    if (index_size >= used_bits) & ((abs_nat = 0) | ((index_size - used_bits) >= (1 shl i)))
      max_n = (1 shl i) shl (index_size - used_bits - 1)
      if (abs_nat < max_n)
        break
      end if
    end if
    index_size = index_size * 2
  end repeat
end macro

macro ebc.insert_index index_size, nat, cst
  local abs_cst, abs_nat, sign, width, nbits
  sign = 0
  nbits = index_size / 8
  if ((cst < 0) & (nat > 0)) | ((cst > 0) & (nat < 0))
     err 'constant and natural values must have the same sign'
  end if
  abs abs_cst, cst
  abs abs_nat, nat
  if (abs_cst <> cst)
    sign = 1
  end if
  width = 8
  if (abs_nat = 0)
    width = 0
  else
    repeat 7
      if (abs_nat < (1 shl (% * nbits)))
        width = %
        break
      end if
    end repeat
  end if
  if (width > 7) | ((width = 7) & (index_size = 16))
    err 'natural value too large for index size'
  end if
  if (abs_cst >= 1 shl (index_size - 4 - (width*2)))
    err 'constant and natural values too large for index size'
  end if
  ; NB: we abuse the fact that 'di' inserts a 'dq' for size 3 or 4
  di (index_size/16), sign shl (index_size-1) + width shl (index_size-4) + abs_cst shl (width*nbits) + abs_nat
end macro

macro ebc.move_imm_ind_rel mov_width, op2_width, ops&
  local op1, op2, op1_reg_num, op1_indirect, op1_index, op2_index, index_size
  local op1_nat, op2_nat, op1_cst, op2_cst, opcode, ins_width, _op2_width, rel
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
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'index specified for direct register operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  _op2_width = op2_width
  if (_op2_width = -1)
    if (mov_width = -1)
      ebc.get_index_size index_size, op2_nat, op2_cst
      if (index_size > 64)
        err 'constant and natural values too large for index encoding'
      end if
      repeat 3 i:1
        if index_size = (8 shl i)
          _op2_width = i
          break
        end if
      end repeat
    else
      is_reloc rel, op2
      if (rel) | (op2 and 0xFFFFFFFF00000000)
        _op2_width = 3
      else if (op2 and 0xFFFF0000)
        _op2_width = 2
      else
        _op2_width = 1
      end if
    end if
  end if

  db _op2_width shl 6 + opcode
  db op1_index shl 6 + ins_width shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  if (mov_width <> -1)
    di _op2_width, op2
  else
    ebc.insert_index (1 shl _op2_width)*8, op2_nat, op2_cst
  end if
end macro

macro ebc.move_general opcode, ops&
  local op1, op2, op1_reg_num, op2_reg_num, op1_indirect, op2_indirect, op2_imm
  local op1_index, op2_index, op1_nat, op2_nat, op1_cst, op2_cst, _opcode
  local index_size, index_size1, index_size2
  op1_index = 0
  op2_index = 0
  op1_nat = 0
  op2_nat = 0
  op1_cst = 0
  op2_cst = 0
  op2_imm = 0
  index_size = 16
  _opcode = opcode and 0xff

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
  if (op1_indirect > 1) | (op2_indirect > 1)
    err 'illegal register'
  end if
  if ((op1_index = 1) & (op1_indirect <> 1)) | ((op2_index = 1) & (op2_indirect <> 1))
    err 'index specified for direct register operation'
  else if (op2_index = 3) & (op2_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op2_index = 1) & (op2_nat = 0) & (op2_cst = 0)
    op2_index = 0
  end if
  if ((opcode >= 0x21) & (opcode <= 0x24)) | (opcode = 0x26) | (opcode = 0x33)
    index_size = 32
  else if (opcode = 0x28)
    index_size = 64
  else if (opcode > 0x100) & ((op1_index = 1) | (op2_index = 1))
    index_size1 = 0
    index_size2 = 0
    if (op1_index = 1)
      ebc.get_index_size index_size1, op1_nat, op1_cst
    end if
    if (op2_index = 1)
      ebc.get_index_size index_size2, op2_nat, op2_cst
    end if
    if (index_size1 < index_size2)
      index_size = index_size2
    else
      index_size = index_size1
    end if
    if (index_size = 64) & (opcode = 0x120)
      _opcode = 0x28
    else if (index_size = 32) & (opcode >= 0x11D) & (opcode <= 0x120)
      _opcode = _opcode + 4 ; switch _opcode to the 'd' version
    else if (index_size = 32) & ((opcode = 0x132) | (opcode = 0x125))
      _opcode = _opcode + 1 ; switch _opcode to the 'd' version
    else if (index_size = 16) & (((opcode >= 0x11D) & (opcode <= 0x120)) | (opcode = 0x132) | (opcode = 0x125))
      ; nothing to do - _opcode is already set for 'w' version
    else
      err 'constant and natural values too large for index encoding'
    end if
  end if

  db op1_index shl 7 + (op2_index and 1) shl 6 + _opcode
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num
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
  local op2_index, op2_nat, op2_cst, op2_imm, op2_imm_reloc
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
  if (op1_indirect > 1) | (op2_indirect > 1)
    err 'illegal register'
  end if
  if (op2_index = 1) & (op2_indirect <> 1)
    err 'index specified for direct register operation'
  end if
  if (op2_index = 3) & (op2_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if
  is_reloc op2_imm_reloc, op2_imm
  if (~op2_imm_reloc) & (op2_nat = 0) & (op2_cst = 0) & (op2_imm = 0)
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

macro ebc.jmp_get_offset jmp_size, offset, ops&
  jmp_size = -1
  match reg (n =, c), ops
    ; indexed => no offset
  else match reg (imm), ops
    ; register + immediate => no offset
  else match addr, ops
    is_reloc rel, addr
    if (rel)
      disp = addr - $
      if (disp and 0x1)
        err 'JMP target must be aligned to word boundary'
      end if
      if ((disp/2) <= 0x80) | ((disp/2) >= -0x80)
        offset = disp/2 - 1
        jmp_size = 8
      else if ((disp-6) < 0x80000000) | ((disp-6) > -0x80000000)
        jmp_size = 32
        offset = disp - 6
      else
        jmp_size = 64
        offset = disp - 10
      end if
    end if
  end match
end macro

macro ebc.jmp_call_push_pop opcode, opcode_flags, ops&
  local op1, op1_reg_num, op1_indirect, op1_index, op1_nat, op1_cst
  local op1_imm, op1_imm_reloc, index_size, _opcode, is_op1_imm
  op1_nat = 0
  op1_cst = 0
  op1_imm = 0
  _opcode = opcode and 0xff
  is_immediate is_op1_imm, ops

  if ((opcode_flags < 0x80) & (opcode and 0x40)) | ((opcode >= 0x100) & (is_op1_imm))
    _opcode = _opcode or 0x40
    index_size = 64
    op1_index = 3
    op1_reg_num = 0
    op1_indirect = 0
    op1_imm = ops
  else
    if (opcode_flags >= 0x80)
      index_size = 16
    else
      index_size = 32
    end if
    match _op1 (n1 =, c1), ops
      op1_index = 1
      define op1 _op1
      define op2 _op2
      op1_nat = n1
      op1_cst = c1
    else match _op1 (_imm), ops
      op1_index = 3
      define op1 _op1
      define op2 _op2
      op1_imm = _imm
    else
      op1_index = 0
      define op1 ops
    end match

    ebc.parse_register op1_reg_num, op1_indirect, op1
    if (op1_indirect > 1)
      err 'illegal register'
    end if
    is_reloc op1_imm_reloc, op1_imm
    if (~op1_imm_reloc) & (op1_nat = 0) & (op1_cst = 0) & (op1_imm = 0)
      op1_index = 0
    end if
    if (op1_index = 1) & (op1_indirect <> 1)
      err 'index specified for direct register operation'
    end if
    if (op1_index = 3) & (op1_indirect = 1)
      err 'immediate value specified for indirect register operation'
    end if
  end if

  db (op1_index and 1) shl 7 + _opcode
  db (opcode_flags and 0x0F) shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index index_size, op1_nat, op1_cst
  else if (op1_index = 3)
    di (index_size/16), op1_imm
  end if
end macro

macro ebc.jmp_short flags, imm8
  local rel, disp
  db flags shl 6 + 0x02, imm8
end macro

macro ebc.jmp_auto flags, ops&
  local jmp_size, offset
  ebc.jmp_get_offset jmp_size, offset, ops
  if (jmp_size > 0)
    if (jmp_size = 8)
       ebc.jmp_short (flags shr 2), offset
     else if (jmp_size = 32)
       ebc.jmp_call_push_pop 0x01, flags, R0(offset)
     else
       ebc.jmp_call_push_pop 0x41, flags, offset
     end if
  else
    ebc.jmp_call_push_pop 0x101, flags, ops
  end if
end macro

macro ebc.cmp_immediate opcode, width, ops&
  local op1, op2, op1_reg_num, op1_indirect, op1_index
  local op1_nat, op1_cst, op2_width, rel
  op1_nat = 0
  op1_cst = 0
  op2_width = width

  match _op1 (n1 =, c1) =, _op2, ops
    op1_index = 1
    define op1 _op1
    define op2 _op2
    op1_nat = n1
    op1_cst = c1
  else match _op1 =, _op2, ops
    op1_index = 0
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'index specified for direct register operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op2_width = -1)
    is_reloc rel, op2
    if (rel) | (op2 and 0xFFFF0000)
      op2_width = 1
    else
      op2_width = 0
    end if
  end if

  db op2_width shl 7 + opcode
  db op1_index shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  di op2_width+1, op2
end macro

macro ebc.load_store_sp opcode, ops&
  local op1_reg_num, op2_reg_num, op1_indirect_or_vm, op2_indirect_or_vm
  match [_op1] =, _op2, ops
    if (opcode <> 0x29)
      err 'syntax error'
    end if
    define op1 _op1
    define op2 _op2
  else match _op1 =, [_op2], ops
    if (opcode <> 0x2A)
      err 'syntax error'
    end if
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect_or_vm, op1
  ebc.parse_register op2_reg_num, op2_indirect_or_vm, op2
  if (opcode = 0x29)
    if (op1_indirect_or_vm <> 2) | (op1_reg_num <> 0)
      err 'illegal register'
    end if
    if (op2_indirect_or_vm <> 0)
      err 'syntax error'
    end if
  else if (opcode = 0x2A)
    if (op2_indirect_or_vm <> 2)
      err 'illegal register'
    end if
    if (op1_indirect_or_vm <> 0)
      err 'syntax error'
    end if
  end if

  db opcode
  db op2_reg_num shl 4 + op1_reg_num
end macro

;
; Instruction list
;

macro ADD? ops&
  ebc.math_general 0x4C, ops
end macro

macro ADD32? ops&
  ebc.math_general 0x0C, ops
end macro

macro ADD64? ops&
  ebc.math_general 0x4C, ops
end macro

macro AND? ops&
  ebc.math_general 0x54, ops
end macro

macro AND32? ops&
  ebc.math_general 0x14, ops
end macro

macro AND64? ops&
  ebc.math_general 0x54, ops
end macro

macro ASHR? ops&
  ebc.math_general 0x59, ops
end macro

macro ASHR32? ops&
  ebc.math_general 0x19, ops
end macro

macro ASHR64? ops&
  ebc.math_general 0x59, ops
end macro

macro BREAK? code
  match , code
    break ; fasmg directive
  else
    db 0x00, code
  end match
end macro

macro CALL? ops&
  ebc.jmp_call_push_pop 0x103, 0000b, ops
end macro

macro CALL32? ops&
  ebc.jmp_call_push_pop 0x03, 0000b, ops
end macro

macro CALL32a? ops&
  ebc.jmp_call_push_pop 0x03, 0001b, ops
end macro

; NB: As per the specs, CALL[EX]64 is always absolute
macro CALL64? ops&
  ebc.jmp_call_push_pop 0x43, 0000b, ops
end macro

macro CALL64a? ops&
  ebc.jmp_call_push_pop 0x43, 0000b, ops
end macro

macro CALLEX? ops&
  ebc.jmp_call_push_pop 0x103, 0010b, ops
end macro

macro CALLEX32? ops&
  ebc.jmp_call_push_pop 0x03, 0010b, ops
end macro

macro CALL32EX? ops&
  ebc.jmp_call_push_pop 0x03, 0010b, ops
end macro

macro CALLEX32a? ops&
  ebc.jmp_call_push_pop 0x03, 0011b, ops
end macro

macro CALL32EXa? ops&
  ebc.jmp_call_push_pop 0x03, 0011b, ops
end macro

macro CALLEX64? ops&
  ebc.jmp_call_push_pop 0x43, 0010b, ops
end macro

macro CALL64EX? ops&
  ebc.jmp_call_push_pop 0x43, 0010b, ops
end macro

macro CALLEX64a? ops&
  ebc.jmp_call_push_pop 0x43, 0010b, ops
end macro

macro CALL64EXa? ops&
  ebc.jmp_call_push_pop 0x43, 0010b, ops
end macro

macro CMPeq? ops&
  ebc.math_general 0x45, ops
end macro

macro CMP32eq? ops&
  ebc.math_general 0x05, ops
end macro

macro CMP64eq? ops&
  ebc.math_general 0x45, ops
end macro

macro CMPlte? ops&
  ebc.math_general 0x46, ops
end macro

macro CMP32lte? ops&
  ebc.math_general 0x06, ops
end macro

macro CMP64lte? ops&
  ebc.math_general 0x46, ops
end macro

macro CMPgte? ops&
  ebc.math_general 0x47, ops
end macro

macro CMP32gte? ops&
  ebc.math_general 0x07, ops
end macro

macro CMP64gte? ops&
  ebc.math_general 0x47, ops
end macro

macro CMPulte? ops&
  ebc.math_general 0x48, ops
end macro

macro CMP32ulte? ops&
  ebc.math_general 0x08, ops
end macro

macro CMP64ulte? ops&
  ebc.math_general 0x48, ops
end macro

macro CMPugte? ops&
  ebc.math_general 0x49, ops
end macro

macro CMP32ugte? ops&
  ebc.math_general 0x09, ops
end macro

macro CMP64ugte? ops&
  ebc.math_general 0x49, ops
end macro

macro CMPIeq? ops&
  ebc.cmp_immediate 0x6D, -1, ops
end macro

macro CMPIweq? ops&
  ebc.cmp_immediate 0x6D, 0, ops
end macro

macro CMPIdeq? ops&
  ebc.cmp_immediate 0x6D, 1, ops
end macro

macro CMPI32eq? ops&
  ebc.cmp_immediate 0x2D, -1, ops
end macro

macro CMPI32weq? ops&
  ebc.cmp_immediate 0x2D, 0, ops
end macro

macro CMPI32deq? ops&
  ebc.cmp_immediate 0x2D, 1, ops
end macro

macro CMPI64eq? ops&
  ebc.cmp_immediate 0x6D, -1, ops
end macro

macro CMPI64weq? ops&
  ebc.cmp_immediate 0x6D, 0, ops
end macro

macro CMPI64deq? ops&
  ebc.cmp_immediate 0x6D, 1, ops
end macro

macro CMPIlte? ops&
  ebc.cmp_immediate 0x6E, -1, ops
end macro

macro CMPIwlte? ops&
  ebc.cmp_immediate 0x6E, 0, ops
end macro

macro CMPIdlte? ops&
  ebc.cmp_immediate 0x6E, 1, ops
end macro

macro CMPI32lte? ops&
  ebc.cmp_immediate 0x2E, -1, ops
end macro

macro CMPI32wlte? ops&
  ebc.cmp_immediate 0x2E, 0, ops
end macro

macro CMPI32dlte? ops&
  ebc.cmp_immediate 0x2E, 1, ops
end macro

macro CMPI64lte? ops&
  ebc.cmp_immediate 0x6E, -1, ops
end macro

macro CMPI64wlte? ops&
  ebc.cmp_immediate 0x6E, 0, ops
end macro

macro CMPI64dlte? ops&
  ebc.cmp_immediate 0x6E, 1, ops
end macro

macro CMPIgte? ops&
  ebc.cmp_immediate 0x6F, -1, ops
end macro

macro CMPIwgte? ops&
  ebc.cmp_immediate 0x6F, 0, ops
end macro

macro CMPIdgte? ops&
  ebc.cmp_immediate 0x6F, 1, ops
end macro

macro CMPI32gte? ops&
  ebc.cmp_immediate 0x2F, -1, ops
end macro

macro CMPI32wgte? ops&
  ebc.cmp_immediate 0x2F, 0, ops
end macro

macro CMPI32dgte? ops&
  ebc.cmp_immediate 0x2F, 1, ops
end macro

macro CMPI64gte? ops&
  ebc.cmp_immediate 0x6F, -1, ops
end macro

macro CMPI64wgte? ops&
  ebc.cmp_immediate 0x6F, 0, ops
end macro

macro CMPI64dgte? ops&
  ebc.cmp_immediate 0x6F, 1, ops
end macro

macro CMPIulte? ops&
  ebc.cmp_immediate 0x70, -1, ops
end macro

macro CMPIwulte? ops&
  ebc.cmp_immediate 0x70, 0, ops
end macro

macro CMPIdulte? ops&
  ebc.cmp_immediate 0x70, 1, ops
end macro

macro CMPI32ulte? ops&
  ebc.cmp_immediate 0x30, -1, ops
end macro

macro CMPI32wulte? ops&
  ebc.cmp_immediate 0x30, 0, ops
end macro

macro CMPI32dulte? ops&
  ebc.cmp_immediate 0x30, 1, ops
end macro

macro CMPI64ulte? ops&
  ebc.cmp_immediate 0x70, -1, ops
end macro

macro CMPI64wulte? ops&
  ebc.cmp_immediate 0x70, 0, ops
end macro

macro CMPI64dulte? ops&
  ebc.cmp_immediate 0x70, 1, ops
end macro

macro CMPIugte? ops&
  ebc.cmp_immediate 0x71, -1, ops
end macro

macro CMPIwugte? ops&
  ebc.cmp_immediate 0x71, 0, ops
end macro

macro CMPIdugte? ops&
  ebc.cmp_immediate 0x71, 1, ops
end macro

macro CMPI32ugte? ops&
  ebc.cmp_immediate 0x31, -1, ops
end macro

macro CMPI32wugte? ops&
  ebc.cmp_immediate 0x31, 0, ops
end macro

macro CMPI32dugte? ops&
  ebc.cmp_immediate 0x31, 1, ops
end macro

macro CMPI64ugte? ops&
  ebc.cmp_immediate 0x71, -1, ops
end macro

macro CMPI64wugte? ops&
  ebc.cmp_immediate 0x71, 0, ops
end macro

macro CMPI64dugte? ops&
  ebc.cmp_immediate 0x71, 1, ops
end macro

macro DIV? ops&
  ebc.math_general 0x50, ops
end macro

macro DIV32? ops&
  ebc.math_general 0x10, ops
end macro

macro DIV64? ops&
  ebc.math_general 0x50, ops
end macro

macro DIVU? ops&
  ebc.math_general 0x51, ops
end macro

macro DIVU32? ops&
  ebc.math_general 0x11, ops
end macro

macro DIVU64? ops&
  ebc.math_general 0x51, ops
end macro

macro EXTNDB? ops&
  ebc.math_general 0x5A, ops
end macro

macro EXTNDB32? ops&
  ebc.math_general 0x1A, ops
end macro

macro EXTNDB64? ops&
  ebc.math_general 0x5A, ops
end macro

macro EXTNDD? ops&
  ebc.math_general 0x5C, ops
end macro

macro EXTNDD32? ops&
  ebc.math_general 0x1C, ops
end macro

macro EXTNDD64? ops&
  ebc.math_general 0x5C, ops
end macro

macro EXTNDW? ops&
  ebc.math_general 0x5B, ops
end macro

macro EXTNDW32? ops&
  ebc.math_general 0x1B, ops
end macro

macro EXTNDW64? ops&
  ebc.math_general 0x5B, ops
end macro

macro JMP? ops&
  ebc.jmp_auto 0001b, ops
end macro

macro JMPa? ops&
  ebc.jmp_call_push_pop 0x101, 0000b, ops
end macro

macro JMPcs? ops&
  ebc.jmp_auto 1101b, ops
end macro

macro JMPacs? ops&
  ebc.jmp_call_push_pop 0x101, 1100b, ops
end macro

macro JMPcc? ops&
  ebc.jmp_auto 1001b, ops
end macro

macro JMPacc? ops&
  ebc.jmp_call_push_pop 0x101, 1000b, ops
end macro

macro JMP32? ops&
  ebc.jmp_call_push_pop 0x01, 0001b, ops
end macro

macro JMP32a? ops&
  ebc.jmp_call_push_pop 0x01, 0000b, ops
end macro

macro JMP32cs? ops&
  ebc.jmp_call_push_pop 0x01, 1101b, ops
end macro

macro JMP32acs? ops&
  ebc.jmp_call_push_pop 0x01, 1100b, ops
end macro

macro JMP32cc? ops&
  ebc.jmp_call_push_pop 0x01, 1001b, ops
end macro

macro JMP32acc? ops&
  ebc.jmp_call_push_pop 0x01, 1000b, ops
end macro

macro JMP64? ops&
  ebc.jmp_call_push_pop 0x41, 0001b, ops
end macro

macro JMP64a? ops&
  ebc.jmp_call_push_pop 0x41, 0000b, ops
end macro

macro JMP64cs? ops&
  ebc.jmp_call_push_pop 0x41, 1101b, ops
end macro

macro JMP64acs? ops&
  ebc.jmp_call_push_pop 0x41, 1100b, ops
end macro

macro JMP64cc? ops&
  ebc.jmp_call_push_pop 0x41, 1001b, ops
end macro

macro JMP64acc? ops&
  ebc.jmp_call_push_pop 0x41, 1000b, ops
end macro

macro JMP8? ops&
  ebc.jmp_short 00b, ops
end macro

macro JMP8cc? ops&
  ebc.jmp_short 10b, ops
end macro

macro JMP8cs? ops&
  ebc.jmp_short 11b, ops
end macro

macro LOADSP? ops&
  ebc.load_store_sp 0x29, ops
end macro

macro MOD? ops&
  ebc.math_general 0x52, ops
end macro

macro MOD32? ops&
  ebc.math_general 0x12, ops
end macro

macro MOD64? ops&
  ebc.math_general 0x52, ops
end macro

macro MODU? ops&
  ebc.math_general 0x53, ops
end macro

macro MODU32? ops&
  ebc.math_general 0x13, ops
end macro

macro MODU64? ops&
  ebc.math_general 0x53, ops
end macro

macro MOV? ops&
  ebc.move_general 0x120, ops
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

macro MOVI? ops&
  ebc.move_imm_ind_rel 3, -1, ops
end macro

macro MOVIb? ops&
  ebc.move_imm_ind_rel 0, -1, ops
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

macro MOVIw? ops&
  ebc.move_imm_ind_rel 1, -1, ops
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

macro MOVId? ops&
  ebc.move_imm_ind_rel 2, -1, ops
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

macro MOVIq? ops&
  ebc.move_imm_ind_rel 3, -1, ops
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

macro MOVIn? ops&
  ebc.move_imm_ind_rel -1, -1, ops
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

macro MOVn? ops&
  ebc.move_general 0x132, ops
end macro

macro MOVnw? ops&
  ebc.move_general 0x32, ops
end macro

macro MOVnd? ops&
  ebc.move_general 0x33, ops
end macro

macro MOVREL? ops&
  ebc.move_imm_ind_rel -2, -1, ops
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

macro MOVsn? ops&
  ebc.move_general 0x125, ops
end macro

macro MOVsnw? ops&
  ebc.move_general 0x25, ops
end macro

macro MOVsnd? ops&
  ebc.move_general 0x26, ops
end macro

macro MUL? ops&
  ebc.math_general 0x4E, ops
end macro

macro MUL32? ops&
  ebc.math_general 0x0E, ops
end macro

macro MUL64? ops&
  ebc.math_general 0x4E, ops
end macro

macro MULU? ops&
  ebc.math_general 0x4F, ops
end macro

macro MULU32? ops&
  ebc.math_general 0x0F, ops
end macro

macro MULU64? ops&
  ebc.math_general 0x4F, ops
end macro

macro NEG? ops&
  ebc.math_general 0x4B, ops
end macro

macro NEG32? ops&
  ebc.math_general 0x0B, ops
end macro

macro NEG64? ops&
  ebc.math_general 0x4B, ops
end macro

macro NOT? ops&
  ebc.math_general 0x4A, ops
end macro

macro NOT32? ops&
  ebc.math_general 0x0A, ops
end macro

macro NOT64? ops&
  ebc.math_general 0x4A, ops
end macro

macro OR? ops&
  ebc.math_general 0x55, ops
end macro

macro OR32? ops&
  ebc.math_general 0x15, ops
end macro

macro OR64? ops&
  ebc.math_general 0x55, ops
end macro

macro POP? ops&
  ebc.jmp_call_push_pop 0x6C, 0x80, ops
end macro

macro POP32? ops&
  ebc.jmp_call_push_pop 0x2C, 0x80, ops
end macro

macro POP64? ops&
  ebc.jmp_call_push_pop 0x6C, 0x80, ops
end macro

macro POPn? ops&
  ebc.jmp_call_push_pop 0x36, 0x80, ops
end macro

macro PUSH? ops&
  ebc.jmp_call_push_pop 0x6B, 0x80, ops
end macro

macro PUSH32? ops&
  ebc.jmp_call_push_pop 0x2B, 0x80, ops
end macro

macro PUSH64? ops&
  ebc.jmp_call_push_pop 0x6B, 0x80, ops
end macro

macro PUSHn? ops&
  ebc.jmp_call_push_pop 0x35, 0x80, ops
end macro

macro RET?
  dw 0x0004
end macro

macro SHL? ops&
  ebc.math_general 0x57, ops
end macro

macro SHL32? ops&
  ebc.math_general 0x17, ops
end macro

macro SHL64? ops&
  ebc.math_general 0x57, ops
end macro

macro SHR? ops&
  ebc.math_general 0x58, ops
end macro

macro SHR32? ops&
  ebc.math_general 0x18, ops
end macro

macro SHR64? ops&
  ebc.math_general 0x58, ops
end macro

macro STORESP? ops&
  ebc.load_store_sp 0x2A, ops
end macro

macro SUB? ops&
  ebc.math_general 0x4D, ops
end macro

macro SUB32? ops&
  ebc.math_general 0x0D, ops
end macro

macro SUB64? ops&
  ebc.math_general 0x4D, ops
end macro

macro XOR? ops&
  ebc.math_general 0x56, ops
end macro

macro XOR32? ops&
  ebc.math_general 0x16, ops
end macro

macro XOR64? ops&
  ebc.math_general 0x56, ops
end macro
