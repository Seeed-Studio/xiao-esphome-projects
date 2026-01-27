# Wiki 锚点配置指南

本文档说明如何在 Wiki 仓库中配置锚点，以便自动同步脚本能够正确更新 YAML 配置代码块。

## 锚点格式

在 Wiki 的 Markdown 文档中，需要在 YAML 代码块内使用注释形式的锚点来包裹需要自动更新的内容：

```markdown
```yaml
# ==== AUTO-SYNC START: <yaml_key> ====
<YAML 配置内容>
# ==== AUTO-SYNC END ====
```
```

### 参数说明

- `<yaml_key>`: 全局唯一的标识符，格式为 `<project-name>/<yaml-filename>`
  - 例如: `xiao-w5500-ethernet-adapter/xiao-w5500-ethernet-adapter.yaml`

### 渲染效果

锚点注释会在渲染后的页面中显示为普通注释，用户可以看到同步边界：

```yaml
# ==== AUTO-SYNC START: xiao-w5500-ethernet-adapter/xiao-w5500-ethernet-adapter.yaml ====
esphome:
  name: seeed-esp32-poe
  ...
# ==== AUTO-SYNC END ====
```

## 完整示例

### 示例 1: W5500 Ethernet Adapter 项目

在 `docs/Sensor/SeeedStudio_XIAO_Gadgets/xiao_w5500_ethernet_adapter/esphome_with_xiao_w5500_ethernet_adapter.md` 中：

```markdown
## ESPHome Configuration

```yaml
# ==== AUTO-SYNC START: xiao-w5500-ethernet-adapter/xiao-w5500-ethernet-adapter.yaml ====
esphome:
  name: seeed-esp32-poe
  friendly_name: "XIAO W5500 Ethernet Adapter V1.2"
  ...
# ==== AUTO-SYNC END ====
```
```

### 示例 2: 多语言支持

在英文文档 `docs/Sensor/SeeedStudio_XIAO_Gadgets/XIAO_Soil_Moisture_Sensor.md` 和中文文档 `docs/zh-CN/Sensor/SeeedStudio_XIAO_Gadgets/cn_XIAO_Soil_Moisture_Sensor.md` 中使用相同的 `yaml_key`：

```markdown
```yaml
# ==== AUTO-SYNC START: xiao-soil-moisture-monitor/xiao-soil-moisture-monitor.yaml ====
esphome:
  name: soil-moisture-monitor
  ...
# ==== AUTO-SYNC END ====
```
```

## 重要注意事项

### 1. yaml_key 必须全局唯一

不要在不同的代码块中使用相同的 `yaml_key`。如果需要在多个文档中同步同一个 YAML 文件，请确保使用相同的 `yaml_key`。

### 2. 锚点必须成对出现

每个 `START` 锚点必须有对应的 `END` 锚点，且两者的 `yaml_key` 必须完全一致。

### 3. 锚点缺失会导致同步失败

如果 `.github/sync-mapping.json` 中配置的目标文档缺少对应的锚点，同步流程将会失败并输出错误信息。这是为了防止不完整的同步。

### 4. 代码块语言必须是 `yaml`

脚本只查找 ` ```yaml ` 代码块，不会处理其他语言或未指定语言的代码块。

### 5. 锚点注释会显示给用户

使用代码块内部注释的优点是：
- 用户可以清楚看到哪些内容由脚本管理
- 注释不会影响 YAML 配置的有效性
- 即使脚本失效，内容仍然可读

缺点是：
- 用户在阅读文档时会看到同步标记
- 如果用户手动编辑这些内容，下次同步会被覆盖

## 当前项目映射

查看 `.github/sync-mapping.json` 文件了解当前配置的映射关系。

| 源 YAML 文件 | yaml_key |
|--------------|----------|
| `projects/xiao-w5500-ethernet-adapter/xiao-w5500-ethernet-adapter.yaml` | `xiao-w5500-ethernet-adapter/xiao-w5500-ethernet-adapter.yaml` |
| `projects/xiao-soil-moisture-monitor/xiao-soil-moisture-monitor.yaml` | `xiao-soil-moisture-monitor/xiao-soil-moisture-monitor.yaml` |
| `projects/xiao_24ghz_mmwave/xiao_24ghz_mmwave.yaml` | `xiao_24ghz_mmwave/xiao_24ghz_mmwave.yaml` |
| `projects/xiao_smart_ir_mate/xiao_smart_ir_mate.yaml` | `xiao_smart_ir_mate/xiao_smart_ir_mate.yaml` |
| ... | ... |

## 测试同步

在正式同步到生产 Wiki 仓库之前，建议先在测试仓库中进行测试：

1. 在测试仓库的对应文档中添加锚点
2. 运行 workflow_dispatch 手动触发同步
3. 检查生成的 PR 内容是否正确
4. 确认无误后切换到生产仓库

## 故障排查

### 同步失败：Anchor not found

**原因**: 目标文档中缺少指定的锚点标记

**解决方法**:
1. 检查错误日志中的 `yaml_key` 和目标文件路径
2. 在目标文档的 YAML 代码块中添加对应的 `START` 和 `END` 锚点
3. 确保 `yaml_key` 与映射配置中的完全一致

### 同步失败：File not found

**原因**: 映射配置中指定的目标文档路径不存在

**解决方法**:
1. 检查 `.github/sync-mapping.json` 中的 `path` 是否正确
2. 确认目标文档在 Wiki 仓库中确实存在

### 代码块格式错误

**原因**: 锚点格式不正确，导致脚本无法正确解析

**解决方法**:
1. 确保 `START` 标记格式为 `# ==== AUTO-SYNC START: <yaml_key> ====`
2. 确保 `END` 标记格式为 `# ==== AUTO-SYNC END ====`
3. 注意标记前后的 `====` 数量要正确
4. 代码块语言必须指定为 `yaml`

### YAML 内容被破坏

**原因**: 手动编辑了锚点之间的内容，或同步过程中出现错误

**解决方法**:
1. 检查 Wiki PR 的 diff，确认变更是否正确
2. 如果内容错误，从源 YAML 文件重新同步
3. 避免手动编辑锚点之间的内容
