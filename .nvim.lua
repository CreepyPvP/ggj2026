function append_path(str, seq)
    local res = vim.fn.fnameescape(vim.fn.expand(str))
    seq[#seq + 1] = res
end

path_seq = {}
append_path(".,", path_seq)
append_path("code,", path_seq)
append_path("external,", path_seq)
-- append_path("external/sdl/include", path_seq)
vim.opt.path = table.concat(path_seq, ",")

-- vim.opt.errorformat = " %#%f(%l\\,%c):\\ %m"

vim.keymap.set("n", "<C-p>", function () vim.fn.jobstart("run.bat") end);
vim.keymap.set("n", "<", function () vim.fn.jobstart("run.bat") end);

-- vim.keymap.set("n", "<C-p>", "<cmd>tabnew term://run.bat<CR>");
vim.keymap.set("n", "<C-b>", "<cmd>make<CR>");
vim.keymap.set("n", "gr", "<cmd>vim <cword> ./code/**<CR>");
